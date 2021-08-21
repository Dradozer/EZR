#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


//layout(rgba32f, binding = 1) uniform image2D bloom;
//layout(rgba32f, binding = 2) uniform image2D alphaness;
//layout(rgba32f, binding = 3) uniform image2D cloudDistance;

layout(rgba32f, binding = 0) uniform image2D img_output;

layout (binding = 1) uniform sampler3D cloudBase;
layout (binding = 2) uniform sampler3D cloudDetail;
layout (binding = 3) uniform sampler2D cloudMotion;
layout (binding = 4) uniform sampler2D weatherMotion;
layout (binding = 5) uniform sampler2D sky;
layout (binding = 6) uniform sampler2D prevTex;
layout (binding = 7) uniform sampler2D terrainTex;


//////////////// STANDARD PARAMS //////////////////////////

uniform vec3 cameraPos;
uniform mat4 inv_proj;
uniform mat4 inv_view;
uniform float frame_time;
uniform vec2 resolution;
uniform int frameCounter;
//////////////// EARTH PARAMS //////////////////////////

uniform float earth_radius;
uniform vec3 earth_pos;
uniform float inner_atmosphere;
uniform float outer_atmosphere;

//////////////// SKY PARAMS //////////////////////////

uniform vec3 zenitColor;
uniform vec3 horizonColor;

//////////////// LIGHT PARAMS //////////////////////////
uniform vec3 lightDir;
uniform float lightFactor;
uniform vec3 realLightColor;
uniform float absorption = 0.0035;

//////////////// CLOUD PARAMS //////////////////////////

uniform vec3 cloudColor;
uniform float cloud_coverage;
uniform float cloud_speed;
uniform float cloud_crispiness;
uniform float cloud_curliness;
uniform float cloud_type;
uniform vec3 cloudColorTop = (vec3(169., 149., 149.)*(1.5/255.));
uniform vec3 cloudColorBottom =  (vec3(65., 70., 80.)*(1.5/255.));
uniform vec3 windDirection;

/////////////// TEXTURE PARAMS ////////////////////////

uniform float baseNoiseScale;
uniform float highFreqNoiseUVScale;
uniform float maxRenderDist;

/////////////////////////////////////////
uniform sampler3D cloud;
uniform sampler3D worley32;
uniform sampler2D weatherTex;
uniform sampler2D depthMap;

uniform bool enablePowder = false;
uniform float densityFactor = 0.02;

uniform bool activateBNoise;

#define BAYER_FACTOR 1.0/16.0
// Cloud types height density gradients
#define STRATUS_GRADIENT vec4(0.0, 0.1, 0.2, 0.3)
#define STRATOCUMULUS_GRADIENT vec4(0.02, 0.2, 0.48, 0.625)
#define CUMULUS_GRADIENT vec4(0.00, 0.1625, 0.88, 0.98)
#define CLOUD_TOP_OFFSET 750.0
#define CLOUDS_MIN_TRANSMITTANCE 1e-1
#define CLOUDS_TRANSMITTANCE_THRESHOLD 1.0 - CLOUDS_MIN_TRANSMITTANCE

// earthRadius = 600000.0;
// sphereInnerRadius = 5000.0;
// sphereOuterRadius = 17000.0;

#define EARTH_RADIUS earth_radius
#define SPHERE_INNER_RADIUS (EARTH_RADIUS + inner_atmosphere)
#define SPHERE_OUTER_RADIUS (SPHERE_INNER_RADIUS + outer_atmosphere)
#define SPHERE_DELTA float(SPHERE_OUTER_RADIUS - SPHERE_INNER_RADIUS)

#define CLOUDS_AMBIENT_COLOR_TOP cloudColorTop
#define CLOUDS_AMBIENT_COLOR_BOTTOM cloudColorBottom

// Cone sampling random offsets

uniform int bayerFilter2[4u] = int[]
(
0, 2, 
3, 1 
);

uniform float bayerFilter[16u] = float[]
(
0.0*BAYER_FACTOR, 8.0*BAYER_FACTOR, 2.0*BAYER_FACTOR, 10.0*BAYER_FACTOR,
12.0*BAYER_FACTOR, 4.0*BAYER_FACTOR, 14.0*BAYER_FACTOR, 6.0*BAYER_FACTOR,
3.0*BAYER_FACTOR, 11.0*BAYER_FACTOR, 1.0*BAYER_FACTOR, 9.0*BAYER_FACTOR,
15.0*BAYER_FACTOR, 7.0*BAYER_FACTOR, 13.0*BAYER_FACTOR, 5.0*BAYER_FACTOR
);

uniform vec3 noiseKernel[6u] = vec3[]
(
vec3( 0.38051305,  0.92453449, -0.02111345),
vec3(-0.50625799, -0.03590792, -0.86163418),
vec3(-0.32509218, -0.94557439,  0.01428793),
vec3( 0.09026238, -0.27376545,  0.95755165),
vec3( 0.28128598,  0.42443639, -0.86065785),
vec3(-0.16852403,  0.14748697,  0.97460106)
);

vec3 sphereCenter = vec3(0.0, -EARTH_RADIUS, 0.0);

float HG( float sundotrd, float g) {
    float gg = g * g;
    return (1. - gg) / pow( 1. + gg - 2. * g * sundotrd, 1.5);
}

bool raySphereintersection(vec3 ro, vec3 rd, float radius, out vec3 startPos){

    float t;

    sphereCenter.xz = cameraPos.xz;

    float radius2 = radius*radius;

    vec3 L = ro - sphereCenter;
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, L);
    float c = dot(L,L) - radius2;

    float discr = b*b - 4.0 * a * c;
    if(discr < 0.0) return false;
    t = max(0.0, (-b + sqrt(discr))/2);
    if(t == 0.0){
        return false;
    }
    startPos = ro + rd*t;

    return true;
}

bool raySphereintersection2(vec3 ro, vec3 rd, float radius, out vec3 startPos){

    float t;
    sphereCenter.xz = cameraPos.xz;
    float radius2 = radius*radius;
    vec3 L = ro - sphereCenter;
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, L);
    float c = dot(L,L) - radius2;

    float discr = b*b - 4.0 * a * c;
    if(discr < 0.0) return false;
    t = max(0.0, (-b + sqrt(discr))/2);
    if(t == 0.0){
        return false;
    }
    startPos = ro + rd*t;
    return true;
}

vec3 getSun(const vec3 d, float powExp){
    float sun = clamp( dot(lightDir,d), 0.0, 1.0 );
    vec3 col = 0.8*vec3(1.0,.6,0.1)*pow( sun, powExp );
    return col;
}

float Random2D(in vec3 st)
{
    return fract(sin(frame_time*dot(st.xyz, vec3(12.9898, 78.233, 57.152))) * 43758.5453123);
}

float getHeightFraction(vec3 inPos){
    return (length(inPos - sphereCenter) - SPHERE_INNER_RADIUS)/(SPHERE_OUTER_RADIUS - SPHERE_INNER_RADIUS);
}


float remap(float originalValue, float originalMin, float originalMax, float newMin, float newMax)
{
    return newMin + (((originalValue - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

float getDensityForCloud(float heightFraction, float cloudType)
{
    float stratusFactor = 1.0 - clamp(cloudType * 2.0, 0.0, 1.0);
    float stratoCumulusFactor = 1.0 - abs(cloudType - 0.5) * 2.0;
    float cumulusFactor = clamp(cloudType - 0.5, 0.0, 1.0) * 2.0;

    vec4 baseGradient = stratusFactor * STRATUS_GRADIENT + stratoCumulusFactor * STRATOCUMULUS_GRADIENT + cumulusFactor * CUMULUS_GRADIENT;


    return smoothstep(baseGradient.x, baseGradient.y, heightFraction) - smoothstep(baseGradient.z, baseGradient.w, heightFraction);

}

float threshold(const float v, const float t)
{
    return v > t ? v : 0.0;
}

vec2 getUVProjection(vec3 p){
    return p.xz/SPHERE_INNER_RADIUS + 0.5;
}

float sampleCloudDensity(vec3 p, bool expensive, float lod){

    float heightFraction = getHeightFraction(p);
    vec3 animation = heightFraction * windDirection * CLOUD_TOP_OFFSET + windDirection * frame_time * cloud_speed;
    vec2 uv = getUVProjection(p);
    vec2 moving_uv = getUVProjection(p + animation);


    if(heightFraction < 0.0 || heightFraction > 1.0){
        return 0.0;
    }

    vec4 low_frequency_noise = texture(cloudBase, vec3(uv*cloud_crispiness, heightFraction));
    float lowFreqFBM = dot(low_frequency_noise.gba, vec3(0.625, 0.25, 0.125));
    float base_cloud = remap(low_frequency_noise.r, -(1.0 - lowFreqFBM), 1.0, 0.0 , 1.0);

    float density = getDensityForCloud(heightFraction, cloud_type);
    base_cloud *= (density/heightFraction);

    vec3 weather_data = texture(weatherMotion, moving_uv).rgb;

    //float cloud_coverage = weather_data.r*cloud_coverage;
    float cloud_coverage = weather_data.r*cloud_coverage;
    float base_cloud_with_coverage = remap(base_cloud , cloud_coverage , 1.0 , 0.0 , 1.0);
    base_cloud_with_coverage *= cloud_coverage;

    if(expensive)
    {
        vec3 erodeCloudNoise = texture(cloudDetail, vec3(moving_uv*cloud_crispiness, heightFraction)*cloud_curliness).rgb;
        float highFreqFBM = dot(erodeCloudNoise.rgb, vec3(0.625, 0.25, 0.125));//(erodeCloudNoise.r * 0.625) + (erodeCloudNoise.g * 0.25) + (erodeCloudNoise.b * 0.125);
        float highFreqNoiseModifier = mix(highFreqFBM, 1.0 - highFreqFBM, clamp(heightFraction * 10.0, 0.0, 1.0));
        base_cloud_with_coverage = base_cloud_with_coverage - highFreqNoiseModifier * (1.0 - base_cloud_with_coverage);
        base_cloud_with_coverage = remap(base_cloud_with_coverage*2.0, highFreqNoiseModifier * 0.2, 1.0, 0.0, 1.0);
    }
    return clamp(base_cloud_with_coverage, 0.0, 1.0);
}


float beer(float d){
    return exp(-d);
}

float powder(float d){
    return (1. - exp(-2.*d));

}

float phase(vec3 inLightVec, vec3 inViewVec, float g) {
    float costheta = dot(inLightVec, inViewVec) / length(inLightVec) / length(inViewVec);
    return HG(costheta, g);
}



float raymarchToLight(vec3 o, float stepSize, vec3 lightDir, float originalDensity, float lightDotEye)
{

    vec3 startPos = o;
    float ds = stepSize * 6.0;
    vec3 rayStep = lightDir * ds;
    const float CONE_STEP = 1.0/6.0;
    float coneRadius = 1.0;
    float density = 0.0;
    float coneDensity = 0.0;
    float invDepth = 1.0/ds;
    float sigma_ds = -ds*absorption;
    vec3 pos;

    float T = 1.0;

    for(int i = 0; i < 6; i++)
    {
        pos = startPos + coneRadius*noiseKernel[i]*float(i);

        float heightFraction = getHeightFraction(pos);
        if(heightFraction >= 0)
        {

            float cloudDensity = sampleCloudDensity(pos, density > 0.3, i/16);
            if(cloudDensity > 0.0)
            {
                float Ti = exp(cloudDensity*sigma_ds);
                T *= Ti;
                density += cloudDensity;
            }
        }
        startPos += rayStep;
        coneRadius += CONE_STEP;
    }
    return T;
}

vec3 ambientlight = vec3(255, 255, 235)/255;

float ambientFactor = 0.5;
vec3 lc = ambientlight * ambientFactor;// * cloud_bright;

vec3 ambient_light(float heightFrac)
{
    return mix( vec3(0.5, 0.67, 0.82), vec3(1.0), heightFrac);
}

vec4 raymarchToCloud(vec3 startPos, vec3 endPos, vec3 bg, out vec4 cloudPos){
    vec3 path = endPos - startPos;
    float len = length(path);

    uvec2 fragCoord = gl_GlobalInvocationID.xy;

    //float maxLen = length(planeDim);

    //float volumeHeight = planeMax.y - planeMin.y;

    //const int nSteps = 64;//int(mix(48.0, 96.0, clamp( len/SPHERE_DELTA - 1.0,0.0,1.0) ));
    const int nSteps = int(mix(32.0, 64.0, clamp( len/SPHERE_DELTA - 1.0,0.0,1.0)));
//    if(activateBNoise) startPos += texture(blueNoise, getUVProjection(startPos)).rg;

    float ds = len/nSteps;
    vec3 dir = path/len;
    dir *= ds;
    vec4 col = vec4(0.0);
    int a = int(fragCoord.x) % 4;
    int b = int(fragCoord.y) % 4;
    startPos += dir * bayerFilter[a * 4 + b];
    //startPos += dir*abs(R)*.5;
    vec3 pos = startPos;

    float density = 0.0;

    float lightDotEye = dot(normalize(lightDir), normalize(dir));

    float T = 1.0;
    float sigma_ds = -ds*densityFactor;
    bool expensive = true;
    bool entered = false;

    int zero_density_sample = 0;

    int count = 0;
    for(int i = 0; i < nSteps; ++i)
    {
        float density_sample = sampleCloudDensity(pos, true, i/16);
        if(density_sample > 0.)
        {
            if(!entered){
                cloudPos = vec4(pos,1.0);
                entered = true;
            }
            float height = getHeightFraction(pos);
            vec3 ambientLight = CLOUDS_AMBIENT_COLOR_BOTTOM; //mix( CLOUDS_AMBIENT_COLOR_BOTTOM, CLOUDS_AMBIENT_COLOR_TOP, height );
            float light_density = raymarchToLight(pos, ds*0.1, lightDir, density_sample, lightDotEye);
            float scattering = mix(HG(lightDotEye, -0.08), HG(lightDotEye, 0.08), clamp(lightDotEye*0.5 + 0.5, 0.0, 1.0));
            //scattering = 0.6;
            scattering = max(scattering, 1.0);
            float powderTerm =  powder(density_sample);
            if(!enablePowder)
            powderTerm = 1.0;

            vec3 S = 0.6*( mix( mix(ambientLight*1.8, bg, 0.2), scattering*realLightColor, powderTerm*light_density)) * density_sample;
            float dTrans = exp(density_sample*sigma_ds);
            vec3 Sint = (S - S * dTrans) * (1. / density_sample);
            col.rgb += T * Sint;
            T *= dTrans;
            count = 0;
        } 
        else {
            if (count > 16) break;
        }

        if( T <= CLOUDS_MIN_TRANSMITTANCE ) break;

        pos += dir;
        //}
    }
    //col.rgb += ambientlight*0.02;
    col.a = 1.0 - T;

    //col = vec4( vec3(getHeightFraction(startPos)), 1.0);

    return col;
}

vec3 computeClipSpaceCoord(vec2 fragCoord){
    vec2 ray_nds = 2.0*vec2(fragCoord.xy)/vec2(resolution.x,resolution.y) - 1.0;
    return vec3(ray_nds, 1.0);
}

vec2 computeScreenPos(vec2 ndc){
    return (ndc*0.5 + 0.5);
}

float computeFogAmount(in vec3 startPos, in float factor){
    float dist = length(startPos - cameraPos);
    float radius = (cameraPos.y - sphereCenter.y) * 0.3;
    float alpha = (dist / radius);
    return (1.-exp( -dist*alpha*factor));
}

bool raySphereintersectionSkyMap(vec3 rd, float radius, out vec3 startPos){
	
	float t;

	vec3 sphereCenter_ = vec3(0.0);

	float radius2 = radius*radius;

	vec3 L = - sphereCenter_;
	float a = dot(rd, rd);
	float b = 2.0 * dot(rd, L);
	float c = dot(L,L) - radius2;

	float discr = b*b - 4.0 * a * c;
	t = max(0.0, (-b + sqrt(discr))/2);

	startPos = rd*t;

	return true;
}

void main()
{
    vec4 fragColor_v, bloom_v, alphaness_v, cloudDistance_v;
    ivec2 fragCoord = ivec2(gl_GlobalInvocationID.xy);

//    ivec2 fragCoordMod = fragCoord % 4; 
//    bool condition = frameCounter == bayerFilter[fragCoordMod.x * 4 + fragCoordMod.y]*16;
//    ivec2 fragCoordMod = fragCoord % 2; 
//    bool condition = frameCounter == bayerFilter2[fragCoordMod.x * 2 + fragCoordMod.y];
    vec4 fragBlackColor = texelFetch(terrainTex, fragCoord, 0);
    if(fragBlackColor!=vec4(0.0,0.0,0.0,1.0))
    {
        imageStore(img_output, fragCoord, fragBlackColor);
        return;
    }
//    if (!condition) {
////                vec4 fragColor = texelFetch(prevTex, fragCoord, 0);
//        //        imageStore(img_output, fragCoord, fragColor);
//        return;
//    }

    //compute ray direction
    vec4 ray_clip = vec4(computeClipSpaceCoord(fragCoord), 1.0);
    vec4 ray_view = inv_proj * ray_clip;
    ray_view = vec4(ray_view.xy, -1.0, 0.0);
    vec3 worldDir = (inv_view * ray_view).xyz;
    worldDir = normalize(worldDir);

    vec3 startPos, endPos;
    vec4 v = vec4(0.0);

    int case_ = 0;
   
    vec4 bg = texture(sky, vec2(fragCoord.x / resolution.x,fragCoord.y / resolution.y));

    vec3 fogRay;
    if(cameraPos.y < SPHERE_INNER_RADIUS - EARTH_RADIUS){
        raySphereintersection(cameraPos, worldDir, SPHERE_INNER_RADIUS, startPos);
        raySphereintersection(cameraPos, worldDir, SPHERE_OUTER_RADIUS, endPos);
        fogRay = startPos;
    }else if(cameraPos.y > SPHERE_INNER_RADIUS - EARTH_RADIUS && cameraPos.y < SPHERE_OUTER_RADIUS - EARTH_RADIUS){
        startPos = cameraPos;
        raySphereintersection(cameraPos, worldDir, SPHERE_OUTER_RADIUS, endPos);
        bool hit = raySphereintersection(cameraPos, worldDir, SPHERE_INNER_RADIUS, fogRay);
        if(!hit)
        fogRay = startPos;
        case_ = 1;
    }else{
        raySphereintersection2(cameraPos, worldDir, SPHERE_OUTER_RADIUS, startPos);
        raySphereintersection2(cameraPos, worldDir, SPHERE_INNER_RADIUS, endPos);
        raySphereintersection(cameraPos, worldDir, SPHERE_OUTER_RADIUS, fogRay);
        case_ = 2;
    }

    //compute fog amount and early exit if over a certain value
    float fogAmount = computeFogAmount(fogRay, 0.00006);

    fragColor_v = bg;
    bloom_v = vec4(getSun(worldDir, 128)*1.3,1.0);

    if(fogAmount > 0.965){
        fragColor_v = bg;
        bloom_v = bg;
        imageStore(img_output, fragCoord, fragColor_v);
        //imageStore(bloom, fragCoord, bloom_v);
        //imageStore(alphaness, fragCoord, vec4(0.0));
        //imageStore(cloudDistance, fragCoord, vec4(-1.0));
        return;
    }

    v = raymarchToCloud(startPos,endPos, bg.rgb, cloudDistance_v);
    //cloudDistance_v = vec4(distance(cameraPos, cloudDistance_v.xyz), 0.0,0.0,0.0);

    float cloudAlphaness = threshold(v.a, 0.2);
    v.rgb = v.rgb*1.8 - 0.1;

    // apply atmospheric fog to far away clouds
    vec3 ambientColor = bg.rgb;

    // use current position distance to center as action radius
    v.rgb = mix(v.rgb, bg.rgb*v.a, clamp(fogAmount,0.,1.));

    // add sun glare to clouds
    float sun = clamp( dot(lightDir,normalize(endPos - startPos)), 0.0, 1.0 );
    vec3 s = 0.8*vec3(1.0,0.4,0.2)*pow( sun, 256.0 );
    v.rgb += s*v.a;

    // blend clouds and background

    bg.rgb = bg.rgb*(1.0 - v.a) + v.rgb;
    bg.a = 1.0;

    fragColor_v = bg;
    alphaness_v = vec4(cloudAlphaness, 0.0, 0.0, 1.0);

    if(cloudAlphaness > 0.1){
        float fogAmount = computeFogAmount(startPos, 0.00003);
        vec3 cloud = mix(vec3(0.0), bloom_v.rgb, clamp(fogAmount,0.,1.));
        bloom_v.rgb = bloom_v.rgb*(1.0 - cloudAlphaness) + cloud.rgb;
    }

    //fragColor_v.a = alphaness_v.r;

    imageStore(img_output, fragCoord, fragColor_v);
    //imageStore(bloom, fragCoord, bloom_v);
    //imageStore(alphaness, fragCoord, alphaness_v);
    //imageStore(cloudDistance, fragCoord, cloudDistance_v);
}
