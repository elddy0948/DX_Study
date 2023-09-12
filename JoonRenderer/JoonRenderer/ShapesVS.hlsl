cbuffer cbPerObject : register(b0)
{
    float4x4 world;
};

cbuffer cbPass : register(b1)
{
    float4x4 view;
    float4x4 invView;
    float4x4 proj;
    float4x4 invProj;
    float4x4 viewProj;
    float4x4 invViewProj;
    
    float3 eyePos;
    float2 renderTargetSize;
    float2 invRenderTargetSize;

    float nearZ;
    float farZ;
    float totalTime;
    float deltaTime;
};

struct VertexIn
{
    float3 posLocal : POSITION;
    float4 color : COLOR;
};


struct VertexOut
{
    float4 posHomog : SV_Position;
    float4 color : COLOR;
};


VertexOut main(VertexIn vin)
{
    VertexOut vout;
    
    float4 posWorld = mul(float4(vin.posLocal, 1.0f), world);
    
    vout.posHomog = mul(posWorld, viewProj);
    vout.color = vin.color;
    
    return vout;
}