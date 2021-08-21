#version 450
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

layout (binding = 1) uniform sampler3D cloudBase;
layout (binding = 2) uniform sampler3D cloudDetail;
layout (binding = 3) uniform sampler2D cloudMotion;

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Intersection {
    vec3 startPoint;
    vec3 endPoint;
    bool valid;
    float t;
};

uniform vec3 SUN_LOCATION;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 inv_proj;
uniform mat4 inv_view;
uniform vec3 cameraPos;
uniform vec2 fovy;

uniform float earth_radius;
uniform vec3 earth_pos;

uniform float inner_atmosphere;
uniform float outer_atmosphere;
uniform float coverage;
uniform float frame_time;

uniform float lightFactor;
uniform vec3 realLightColor;
uniform vec3 cloudColor;
uniform vec3 zenitColor;
uniform vec3 horizonColor;
uniform vec3 lightDir;
uniform float baseNoiseScale;
uniform float highFreqNoiseUVScale;
uniform vec3 windDirection;
uniform float maxRenderDist;


#define CLOUD_TOP_OFFSET 750.0
#define CLOUD_SPEED 1.25

#define highFreqNoiseHScale 4.0f
#define highFreqNoiseScale 1.0f

#define BAYER_FACTOR 1.0/16.0
#define STRATUS_GRADIENT vec4(0.0, 0.1, 0.2, 0.3)
#define STRATOCUMULUS_GRADIENT vec4(0.02, 0.2, 0.48, 0.625)
#define CUMULUS_GRADIENT vec4(0.00, 0.1625, 0.88, 0.98)
#define WIND_DIRECTION (vec3(1,0,0))
#define COVERAGE_MULTIPLIER 0.3
#define CONE_STEP 0.1666666

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

vec3 ambientLight()
{
    vec3 lightColor = realLightColor;
    vec3 ambientColor = mix(horizonColor, zenitColor, 0.15);
    return mix(lightColor, ambientColor, 0.65) * lightFactor * 0.65;
}


float henyeyGreenstein(vec3 l, vec3 v, float g, float ca)
{
    float g2 = g * g;

    return ((1.0 - g2) / pow((1.0 + g2 - 2.0 * g * ca), 1.5 )) * (1.0 / (4.0 * 3.1415));
}

// Intensity decreases with density
float beer(float density)
{
    return exp(-density);
}

// Beer's law inverted equation for edges
float powder(float density, float ca)
{
    float f = 1.0 - exp(-density * 2.0);
    return mix(1.0, f, clamp(-ca * 0.5 + 0.5, 0.0, 1.0));
}

float lightEnergy(vec3 l, vec3 v, float ca, float coneDensity)
{
    return 50.0 * beer(coneDensity) * powder(coneDensity, ca) * henyeyGreenstein(l, v, 0.2, ca);
}

float rand(float n){return fract(sin(n) * 43758.5453123);}

// Maps values from one range to another
float remap(in float value, in float original_min, in float original_max, in float new_min, in float new_max)
{
	return new_min + ( ((value - original_min) / (original_max - original_min)) * (new_max - new_min) );
}

float remapClamped(in float value, in float original_min, in float original_max, in float new_min, in float new_max)
{
    float t = new_min + ( ((value - original_min) / (original_max - original_min)) * (new_max - new_min) );
    return clamp(t, new_min, new_max);
}

float remapClampedBeforeAndAfter(in float value, in float original_min, in float original_max, in float new_min, in float new_max)
{   
    value = clamp(value, original_min, original_max);
    float t = new_min + ( ((value - original_min) / (original_max - original_min)) * (new_max - new_min) );
    return clamp(t, new_min, new_max);
}

float remapValue(float original, float oMin, float oMax, float nMin, float nMax)
{
    return nMin + ((original - oMin) / (oMax - oMin)) * (nMax - nMin);
}


float getRelativeHeightInAtmosphere(in vec3 point, in vec3 earthCenter, in vec3 startPosOnInnerShell, in vec3 rayDir, in vec3 eye)
{
	float lengthOfRayfromCamera = length(point - eye);
	float lengthOfRayToInnerShell = length(startPosOnInnerShell - eye);
	vec3 pointToEarthDir = normalize(point - earthCenter);
	// assuming RayDir is normalised
	float cosTheta = dot(rayDir, pointToEarthDir);

    // CosTheta is an approximation whose error gets relatively big near the horizon and could lead to problems.
    // However, the actual calculationis involve a lot of trig and thats expensive;
    // No longer drawing clouds that close to the horizon and so the cosTheta Approximation is fine

	float numerator = abs(cosTheta * (lengthOfRayfromCamera - lengthOfRayToInnerShell));
	return numerator/(outer_atmosphere-inner_atmosphere);
	// return clamp( length(point.y - projectedPos.y) / ATMOSPHERE_THICKNESS, 0.0, 1.0);
}

vec3 getRelativePositionInAtmosphere(in vec3 pos, in vec3 earthCenter)
{
   	return vec3( ( pos - vec3(earthCenter.x, inner_atmosphere - earth_radius, earthCenter.z) )/ (outer_atmosphere-inner_atmosphere) );
}

vec2 sphericalUVProj(vec3 p)
{
    vec3 dirVector = normalize(p - earth_pos);
    return (dirVector.xz + 1.0) / 2.0;
}


float getHeightFraction(vec3 p)
{
    return (length(p - earth_pos) - inner_atmosphere) / (outer_atmosphere - inner_atmosphere);
}

float getDensityForCloud(float heightFraction, float cloudType)
{
    float stratusFactor = 1.0 - clamp(cloudType * 2.0, 0.0, 1.0);
    float stratoCumulusFactor = 1.0 - abs(cloudType - 0.5) * 2.0;
    float cumulusFactor = clamp(cloudType - 0.5, 0.0, 1.0) * 2.0;

    vec4 baseGradient = stratusFactor * STRATUS_GRADIENT + stratoCumulusFactor * STRATOCUMULUS_GRADIENT + cumulusFactor * CUMULUS_GRADIENT;
    float result = remapValue(heightFraction, baseGradient.x, baseGradient.y, 0.0, 1.0) * remapValue(heightFraction, baseGradient.z, baseGradient.w, 1.0, 0.0);
    return result;
}

float sampleCloudDensity(vec3 p, bool expensive, float heightFraction)
{

    p += heightFraction * windDirection * CLOUD_TOP_OFFSET;
    p += windDirection * CLOUD_SPEED;

    float deltaDist = clamp(length(p - cameraPos), 0.0, 1.0);

    //vec3 weatherData = getWeatherData(p);

    vec2 uv = sphericalUVProj(p);

    // Sample base cloud shape noises (Perlin-Worley + 3 Worley)
    vec4 baseCloudNoise = texture(cloudBase, vec3(uv * baseNoiseScale, heightFraction));

    // Build the low frequency fbm modifier
    float lowFreqFBM = ( baseCloudNoise.g * 0.625) + ( baseCloudNoise.b * 0.25 ) + ( baseCloudNoise.a * 0.125 );
    float baseCloudShape = remapValue(baseCloudNoise.r, -(1.0 - lowFreqFBM), 1.0, 0.0, 1.0);

    // Apply density gradient based on cloud type
    float densityGradient = getDensityForCloud(heightFraction, 1.0f);
    baseCloudShape *= densityGradient / heightFraction;

    // Apply coverage
    float coverage_m = clamp(rand(uv.x), 0.0, 1.0) * coverage;
    // Make sure cloud with less density than actual coverage dissapears
    float coveragedCloud = remapValue(baseCloudShape, coverage_m, 1.0, 0.0, 1.0);
    coveragedCloud *= coverage_m;
    coveragedCloud *= mix(1.0, 0.0, clamp(heightFraction / coverage_m, 0.0, 1.0));

    float finalCloud = coveragedCloud;

    // Only erode the cloud if erosion will be visible (low density sampled until now)
    if(expensive)
    {
        // Build−high frequency Worley noise FBM.
        vec3 erodeCloudNoise = texture(cloudDetail, vec3(uv * highFreqNoiseUVScale, heightFraction * highFreqNoiseHScale) * highFreqNoiseScale).rgb;
        float highFreqFBM = (erodeCloudNoise.r * 0.625) + (erodeCloudNoise.g * 0.25) + (erodeCloudNoise.b * 0.125);

        // Recompute height fraction after applying wind and top offset
        heightFraction = getHeightFraction(p);

        float highFreqNoiseModifier = mix(highFreqFBM, 1.0 - highFreqFBM, clamp(heightFraction * 8.5, 0.0, 1.0));
        finalCloud = remapValue(coveragedCloud, highFreqNoiseModifier * 0.8, 1.0, 0.0, 1.0);
    }

    return clamp(finalCloud, 0.0, 1.0);
}

float sampleLowFrequency(vec3 point, in float relativeHeight)
{
    //Read in the low-frequency Perlin-Worley noises and Worley noises

    // TODO: ADD WIND INFLUENCE HERE
    //point += WIND_DIRECTION * frame_time * CLOUD_SPEED;
//    const float height_fraction = getHeightFraction(point);
    const float height_fraction = 0;
    //vec2 uv = sphericalUVProj(point);
    vec4 lowFrequencyNoises = texture(cloudBase, point);

    //Build an FBM out of the low-frequency Worley Noises that are used to add detail to the Low-frequency Perlin Worley noise
    float lowFrequencyFBM = (lowFrequencyNoises.g * 0.625) + 
                            (lowFrequencyNoises.b * 0.25)  + 
                            (lowFrequencyNoises.a * 0.125);

    lowFrequencyFBM = clamp(lowFrequencyFBM, 0.0, 1.0);                      

    // Define the base cloud shape by dilating it with the low-frequency FBM
    float baseCloud = remapValue( lowFrequencyNoises.r, -(1.0 - lowFrequencyFBM), 1.0, 0.0, 1.0 );

    float densityGradient = getDensityForCloud(height_fraction,rand(0.5));
//    baseCloud *= densityGradient;

    float coverage_val = clamp(rand(0.5),0.0,1.0) * coverage;
    float coverage_cloud = remapValue(baseCloud,coverage_val,1.0,0.0,1.0);
    coverage_cloud *= coverage_val;
    coverage_cloud *= mix(1.0,0.0,clamp(height_fraction/coverage_val,0.0,1.0));

    float final_cloud = coverage_cloud;

    // Use remap to apply the cloud coverage attribute.
//    float base_cloud_with_coverage = remapClampedBeforeAndAfter ( baseCloud, coverage_cloud, 1.0, 0.0, 1.0);


    //base_cloud_with_coverage *= cloud_coverage;

//    return clamp(final_cloud,0.0,1.0);
    return lowFrequencyNoises.r;
}

Intersection raySphereIntersection(in vec3 rO, in vec3 rD, in vec3 sphereCenter, in float sphereRadius)
{
    Intersection isect;
    isect.valid = false;
    isect.startPoint = vec3(0.0);
    isect.endPoint = vec3(0.0);

    // Transform Ray such that the spheres move down, such that the camera is close to the sky dome
    // Only change sphere origin because you can't translate a direction
    rO -= sphereCenter;
    rO /= sphereRadius;

    float A = dot(rD, rD);
    float B = 2.0*dot(rD, rO);
    float C = dot(rO, rO) - 1.0; //uniform sphere
    float discriminant = B*B - 4.0*A*C;

    //If the discriminant is negative, then there is no real root
    if(discriminant < 0.0)
    {
        return isect;
    }

    float t = (-B - sqrt(discriminant))/(2.0*A);
    
    if(t < 0.0) 
    {
        t = (-B + sqrt(discriminant))/(2.0*A);
    }

    if(t >= 0.0)
    {
        vec3 p_start = vec3(rO + t*rD);
        vec3 p_end = vec3(rO - t*rD);
        isect.valid = true;

        p_start *= sphereRadius;
        p_start += sphereCenter;
        p_end *= sphereRadius;
        p_end += sphereCenter;

        isect.startPoint = p_start;
        isect.endPoint = p_end;
    }

    return isect;
}

bool intersectSphere(vec3 o, vec3 d, out vec3 minT, out vec3 maxT)
{
    // Intersect inner sphere
    vec3 sphereToOrigin = (o - earth_pos);
    float b = dot(d, sphereToOrigin);
    float c = dot(sphereToOrigin, sphereToOrigin);
    float sqrtOpInner = b*b - (c - inner_atmosphere * inner_atmosphere);

    // No solution (we are outside the sphere, looking away from it)
    float maxSInner;
    if(sqrtOpInner < 0.0)
    {
        return false;
    }

    float deInner = sqrt(sqrtOpInner);
    float solAInner = -b - deInner;
    float solBInner = -b + deInner;

    maxSInner = max(solAInner, solBInner);

    if(maxSInner < 0.0) return false;

    maxSInner = maxSInner < 0.0? 0.0 : maxSInner;

    // Intersect outer sphere
    float sqrtOpOuter = b*b - (c - outer_atmosphere * outer_atmosphere);

    // No solution - same as inner sphere
    if(sqrtOpOuter < 0.0)
    {
        return false;
    }

    float deOuter = sqrt(sqrtOpOuter);
    float solAOuter = -b - deOuter;
    float solBOuter = -b + deOuter;

    float maxSOuter = max(solAOuter, solBOuter);

    if(maxSOuter < 0.0) return false;

    maxSOuter = maxSOuter < 0.0? 0.0 : maxSOuter;

    // Compute entering and exiting ray points
    float minSol = min(maxSInner, maxSOuter);

    if(minSol > maxRenderDist)
    {
        return false;
    }

    float maxSol = max(maxSInner, maxSOuter);

    minT = o + d * minSol;
    maxT = o + d * maxSol;

    return true;
}

float raymarchToLight(vec3 pos, vec3 d, float stepSize)
{
    vec3 startPos = pos;
    // Modify step size to take as much info as possible in 6 steps
    vec3 rayStep = lightDir * (stepSize) * 0.7;
    // Starting cone radius. Will increase as we move along it
    float coneRadius = 1.0;
    // Sampled density until now
    float invDepth = 1.0 / (stepSize * 6);
    float density = 0.0;
    // Density - transmittance until now
    float coneDensity = 0.0;

    // Light raymarchToLight
    // 6 close samples
    int i = 0;
    while(i < 6)
    {
        // Get position inside cone
        vec3 posInCone = startPos + lightDir + coneRadius * noiseKernel[i] * float(i);

        float deltaDist = clamp(length(posInCone - cameraPos), 0.0, 1.0);  //MODIFYED

        // By advancing towards the light we might go outside the atmosphere
        float heightFraction = getHeightFraction(posInCone);

        if(heightFraction <= 1.0)
        {
            // sample the expensive way if we hare near borders (where density is low, like 0.3 or below)
            float cloudDensity = sampleCloudDensity(posInCone, coneDensity < 0.3, heightFraction);
            if(cloudDensity > 0.0)
            {
                density += cloudDensity;
                float transmittance = 1.0 - (density * invDepth);
                coneDensity += (cloudDensity * transmittance);
            }
        }

        startPos += rayStep;
        coneRadius += CONE_STEP;
        i++;
    }

    // 1 far sample for shadowing
    pos += (rayStep * 8.0);
    float heightFraction = getHeightFraction(pos);
    float cloudDensity = sampleCloudDensity(pos, false, heightFraction);
    if(cloudDensity > 0.0)
    {
        density += cloudDensity;
        float transmittance = 1.0 - (density * invDepth);
        coneDensity += (cloudDensity * transmittance);
    }

    float ca = dot(lightDir, d);

    // Compute light energy arriving at point
    return lightEnergy(lightDir, d, ca, coneDensity);
}

float rayMarchTest(in vec3 start_pos,in vec3 end_pos, out vec3 color){
    // Sampling parameters calculation
    vec3 path = end_pos - start_pos;

    //int sampleCount = int(ceil(mix(48.0, 96.0, clamp((length(path) / (outerSphereRadius - innerSphereRadius)), 0.0, 1.0))));
    int sampleCount = 64;
    // Ray march data
    vec3 stepVector = path / float(sampleCount - 1);
    float stepSize = length(stepVector);
    vec3 viewDir = normalize(path);

    vec3 pos = start_pos;
    float density = 0.0;
    vec4 result = vec4(0.0);

    //float lodAlpha = clamp(length(startPos - camPos) / maxRenderDist, 0.0, 1.0);
    //float samplingLod = mix(0.0, 6.0, lodAlpha);
    //(length(startPos) - innerSphereRadius) / (outerSphereRadius - innerSphereRadius));

    // Dithering on the starting ray position to reduce banding artifacts
    int a = int(gl_GlobalInvocationID.x) % 4;
    int b = int(gl_GlobalInvocationID.y) % 4;
    pos += stepVector * bayerFilter[a * 4 + b];

    // ambient lighting attenuation factor
    float ambientFactor =  max(min(lightFactor, 1.0), 0.1);
    // full lighting
    vec3 lc = realLightColor * lightFactor * cloudColor;
    vec3 ambientL = ambientLight();// vec3(0.8) * lightFactor;

    int i = 0;
    while(i < sampleCount)
    {
        float cloudDensity = sampleCloudDensity(pos, true, getHeightFraction(pos));

        if(cloudDensity > 0.0)	// IF WE HAVE DENSITY, LAUNCH LIGHT SAMPLING
        {
            density += cloudDensity;
            float lightEnergy = raymarchToLight(pos, viewDir, stepSize); // SAMPLE LIGHT

            float height = getHeightFraction(pos);
            vec4 src = vec4(lc * lightEnergy + ambientL, cloudDensity); // ACCUMULATE
            src.rgb *= src.a;
            result = (1.0 - result.a) * src + result;

            if(result.a >= 0.95) // EARLY EXIT ON FULL OPACITY
            break;
        }

        pos += stepVector;
        i++;
    }

    // Return alpha (density) and store final color
    color = result.xyz;
    return result.a;
}

vec3 computeClipSpaceCoord(uvec2 fragCoord){
	vec2 ray_nds = 2.0*vec2(fragCoord.xy)/vec2(1000,800) - 1.0; // MANIPULATE ME
	return vec3(ray_nds, 1.0);
}

void main() {
  // base pixel colour for image
  vec4 pixel = vec4(gl_GlobalInvocationID.xy / 512.f, 0, 0);
  vec4 color = vec4(0,0,0,1);
    //vec4 pixel = vec4(texelFetch(cloudBase, ivec3(gl_GlobalInvocationID.xy, 0), 0).xyz, 0);
  // get index in global work group i.e x,y position
  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);


  //compute ray direction
  vec4 ray_clip = vec4(computeClipSpaceCoord(pixel_coords), 1.0);
  vec4 ray_view = inv_proj * ray_clip;
  ray_view = vec4(ray_view.xy, -1.0, 0.0);
  vec3 worldDir = (inv_view * ray_view).xyz;
  //worldDir = normalize(worldDir);


  // check if current ray direction is below horizon or above
  vec3 start_pos;
  vec3 end_pos;
  //Intersection intersect = raySphereIntersection(cameraPos, worldDir, earth_pos, earth_radius);
  vec4 ambientColor = vec4(mix(horizonColor, zenitColor, 0.15), 0.6);

  bool intersect = intersectSphere(cameraPos, worldDir, start_pos, end_pos);

  if(intersect){
      vec3 color_s;
      float rayMarchResult = rayMarchTest(start_pos,end_pos,color_s);


      rayMarchResult = clamp(rayMarchResult, 0.0, 1.0);

      vec4 finalColor = vec4(color_s, rayMarchResult);
      float alpha = length(start_pos - cameraPos) / maxRenderDist;
      alpha *= alpha;
      alpha = clamp(alpha, 0, 1);
      finalColor = mix(finalColor, ambientColor * lightFactor, alpha);
      color = finalColor;
      //outPos = vec4(start_pos, 1.0);

      //color = vec4(1,1,1,alpha);
      imageStore(img_output, pixel_coords, color);
      return;
  }
  else {
      color = ambientColor * lightFactor;
      imageStore(img_output, pixel_coords, color);
  }
}