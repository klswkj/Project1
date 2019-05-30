//==============================================================
//To Calculate distnace between the nearest and farthest objects
//in a scene that appear acceptably sharp in an image
//===============================================================

Texture2D<float>    tDepth : register(t0);
Texture2D<float>    tCoc : register(t0);
RWTexture2D<float>  uCoC : register(u0);

cbuffer CalcDOFParams
{
    uint2 ScreenParams;
    float zNear;
    float zFar;
    float focusDistance;
    float fStop;
    float focalLength;
    float maxRadius;
    float forceCoC;
};

float CocFromDepth(float sceneDepth, float focusDistance, float fStop, float focalLength)
{
    const float cocScale             = (focalLength * focalLength) / fStop;
    const float distanceToFocusPlane = distanceToLens - focusDistance;
	const float distanceToLens = sceneDepth - focalLength;
    float       coc                  = (distanceToLens > 0.0) ? (cocScale * (distanceToFocusPlane / distanceToLens)) : 0.0;

    coc = clamp(coc * float(ScreenParams.x) * 0.5, -maxRadius, maxRadius);

    return coc;
}

///////////////////////////////////////
// compute camera-space depth for current pixel
float CameraDepth(float depth, float zNear, float zFar)
{
    float invRange = 1.0 / (zFar - zNear);
    return (-zFar * zNear) * invRange / (depth - zFar * invRange);
}


[numthreads(32, 1, 1)]
void CalcDOF(uint3 threadID : SV_DispatchThreadID)
{
    if ((threadID.x < ScreenParams.x) && (threadID.y < ScreenParams.y))
    {
        const float depth    = tDepth.Load(int3(threadID.xy, 0), 0);
        const float camDepth = CameraDepth(depth, zNear, zFar);
        float       CoC      = clamp(CocFromDepth(camDepth, focusDistance, fStop, focalLength), -maxRadius, maxRadius);
        if (abs(forceCoC) > 0.25)
        {
            CoC = -forceCoC;
        }
        uCoC[int2(threadID.xy)] = CoC;
    }
}
