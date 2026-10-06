cbuffer ViewConstants : register(b0) // FConstants
{
    row_major float4x4 InvProjection;
    row_major float4x4 InvView;
    float4 FogColor;
    float2 InvViewportSize;
    float2 Padding0;
    float3 CameraLocation;
    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;
    float FogZ;
    float Padding1;
}

Texture2D<float> SceneDepth : register(t0);
Texture2D<float4> SceneColor : register(t1);

struct PS_INPUT
{
    float4 position : SV_POSITION;
};

// Vertex Shader
PS_INPUT mainVS(uint vertex_id : SV_VertexID)
{
    PS_INPUT output;
	
    float2 full_screen_quad[4] =
    {
        float2(-1.0, 1.0),
		float2(1.0, 1.0),
		float2(1.0, -1.0),
		float2(-1.0, -1.0)
    };
    
    uint indices[] = { 0, 1, 2, 0, 2, 3 }; // Two triangles to form a quad
	
    output.position = float4(full_screen_quad[indices[vertex_id]], 0.0, 1.0);

    return output;
}

float Hdensity(float H)
{
    return mul(FogDensity, exp(mul(-FogHeightFalloff, H - FogZ)));
}

// Pixel Shader
float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float depth = SceneDepth.Load(int3(input.position.x, input.position.y, 0));
    float2 uv = input.position.xy * InvViewportSize;
    float2 ndcXY = float2(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0);
    
    float4 ndc = float4(ndcXY, depth, 1);
    float4 viewposition = mul(ndc, InvProjection);
    float4 worldposition = mul(viewposition / viewposition.w, InvView);
    
    float avgdensity = mul(Hdensity(CameraLocation.z), (1 - exp(mul(-FogHeightFalloff, (worldposition.z - CameraLocation.z)))) / mul(FogHeightFalloff, worldposition.z - CameraLocation.z));
    float distance = length(worldposition.xyz - CameraLocation.xyz);
    float FogFactor = exp(mul(-avgdensity, distance - StartDistance));
    
    float4 FinalColor = mul(SceneColor.Load(int3(input.position.x, input.position.y, 0)), FogFactor) + mul(FogColor, 1 - FogFactor);
    
    return FinalColor;
}