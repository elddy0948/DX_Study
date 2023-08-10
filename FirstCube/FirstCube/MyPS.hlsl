struct PixelIn
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(PixelIn pin) : SV_Target
{
    return pin.Color;
}