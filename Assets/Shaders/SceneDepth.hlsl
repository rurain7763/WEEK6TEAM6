cbuffer ViewConstants : register(b0) // FConstants
{
    row_major float4x4 InvProjection;
    float2 InvViewportSize;
    float DisplayNear;
    float DisplayFar;
}

Texture2D<float> SceneDepth : register(t0);

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

// Pixel Shader
float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float4 output;
    
    float depth = SceneDepth.Load(int3(input.position.x, input.position.y, 0));
    float2 uv = input.position.xy * InvViewportSize;
    float2 ndcXY = float2(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0);
    
    float4 ndc = float4(ndcXY, depth, 1);
    float4 viewposition = mul(ndc, InvProjection);
    float viewdepth = viewposition.z / viewposition.w;
    
    float gray = saturate((viewdepth - DisplayNear) / (DisplayFar - DisplayNear));
    
    return float4(gray, gray, gray, 1.0);
}