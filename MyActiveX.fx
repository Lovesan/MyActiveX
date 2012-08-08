cbuffer VSBuffer : register(b0)
{
  float4x4 world;
  float4x4 worldViewProj;  
};

void SceneVS(
        float3 pos : POSITION,
        float3 norm : TEXCOORD,
        out float4 oPos : SV_POSITION,
        out float3 oNorm : TEXCOORD)
{
    oPos = mul(worldViewProj, float4(pos, 1));
    oNorm = mul(world, norm);
}

float4 ScenePS(
         float4 pos : SV_POSITION,
         float3 n : TEXCOORD)
    : SV_TARGET
{
    float3 colorUp = saturate(dot(float3(0, 1, 0), n) * float3(1, 0.9, 0));
    float3 colorDown = saturate(dot(float3(0, -1, 0), n) * float3(1, 0, 0));
    return float4(colorUp + colorDown, 1);
}
