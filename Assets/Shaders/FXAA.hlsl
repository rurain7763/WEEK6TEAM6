cbuffer Constants : register(b0)
{
    float2 InvResolution;
    float EdgeThreshold;
    float EdgeThresholdMin;
    float SubPiexelStrength;
}

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D color_texture : register(t0);

SamplerState default_sampler : register(s0);

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
    
    float2 uv_coords[4] =
    {
        float2(0.0, 0.0),
        float2(1.0, 0.0),
        float2(1.0, 1.0),
        float2(0.0, 1.0)
    };
    
    uint indices[] = { 0, 1, 2, 0, 2, 3 }; // Two triangles to form a quad
	
    output.position = float4(full_screen_quad[indices[vertex_id]], 0.0, 1.0);
    output.uv = uv_coords[indices[vertex_id]];

    return output;
}

float getluma(float3 a)
{
    return dot(a, float3(0.299, 0.587, 0.114));
}

float4 getsamplecolor(float2 a)
{
    return color_texture.Sample(default_sampler, a);
}

float score(float2 a, float2 b, float2 c)
{
    float4 acolor = getsamplecolor(a);
    float4 bcolor = getsamplecolor(b);
    float4 ccolor = getsamplecolor(c);
    
    float aluma = getluma(acolor.xyz);
    float bluma = getluma(bcolor.xyz);
    float cluma = getluma(ccolor.xyz);
    
    return abs(aluma - 2.0 * bluma + cluma);
}

// Pixel Shader
float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float x[4] = { InvResolution.x, 0, -InvResolution.x, 0};
    float y[4] = { 0, -InvResolution.y, 0, InvResolution.y};
    
    float4 color = color_texture.Sample(default_sampler, input.uv);
    
    float lmax = -99999.0f;
    float lmin = 99999.0f;
       
    for (int i = 0; i < 4; i++)
    {
        float2 sampleuv = { input.uv.x + x[i], input.uv.y + y[i] };
        if(sampleuv.x > 1.0 && sampleuv.x < 0.0 && sampleuv.y > 1.0 && sampleuv.y < 0.0)
        {
            return color;
        }
        float4 samplecolor = getsamplecolor(sampleuv);
        float luma = getluma(samplecolor.xyz);
        lmax = max(lmax, luma);
        lmin = min(lmin, luma);
    }
    
    float localcontrast = lmax - lmin;
    float threshold = max(EdgeThresholdMin, lmax * EdgeThreshold);
    if(localcontrast < threshold)
    {
        return color;
    }
    
    float2 NW = float2(input.uv.x - InvResolution.x, input.uv.y - InvResolution.y);
    float2 N = float2(input.uv.x, input.uv.y - InvResolution.y);
    float2 NE = float2(input.uv.x + InvResolution.x,input.uv.y - InvResolution.y);
    float2 W = float2(input.uv.x - InvResolution.x,input.uv.y);
    float2 A = input.uv;
    float2 E = float2(input.uv.x + InvResolution.x,input.uv.y);
    float2 SW = float2(input.uv.x - InvResolution.x,input.uv.y + InvResolution.y);
    float2 S = float2(input.uv.x,input.uv.y + InvResolution.y);
    float2 SE = float2(input.uv.x + InvResolution.x,input.uv.y + InvResolution.y);
    
    float Hscore = score(NW, N, NE) + score(W, A, E) + score(SW, S, SE);
    float Vscore = score(NW, W, SW) + score(N, A, S) + score(NE, E, SE);
    
    bool horizontal = Hscore >= Vscore;

    float negativeLuma = horizontal ? getluma(getsamplecolor(N).xyz) : getluma(getsamplecolor(W).xyz); // 위 or 왼쪽
    float positiveLuma = horizontal ? getluma(getsamplecolor(S).xyz) : getluma(getsamplecolor(E).xyz); // 아래 or 오른쪽

    bool chooseNegative = abs(negativeLuma - getluma(getsamplecolor(A).xyz)) >= abs(positiveLuma - getluma(getsamplecolor(A).xyz));
    
    float neighborLuma = chooseNegative ? negativeLuma : positiveLuma;

    float2 normalUV = horizontal ? float2(0, InvResolution.y) : float2(InvResolution.x, 0); // A의 이웃으로 옮길때 쓸 값

    if (chooseNegative)
    {
        normalUV = -normalUV;
    }

    float2 tangentUV = horizontal ? float2(InvResolution.x, 0) : float2(0, InvResolution.y); // A의 이웃에서 탐색할 축
    float referenceLuma = (getluma(getsamplecolor(A).xyz) + neighborLuma) * 0.5f; // A의 이웃 luma
    float stopThreshold = abs(getluma(getsamplecolor(A).xyz) - neighborLuma) * 0.25f;
    
    float2 searchbase = input.uv + normalUV * 0.5f; // A의 이웃 위치
    
    // 탐색 시작
    
    // 양쪽으로 움직일 단위 (픽셀)
    float dNeg = 1.0; // 위 혹은 왼쪽
    float dPos = 1.0; // 아래 혹은 오른쪽
    
    // 탐색 밝기와 기존 밝기 차이
    float deltaNeg = 0.0f;
    float deltaPos = 0.0f;
    
    bool doneNeg = false;
    bool donePos = false;
    
    const int MAX_SEARCH = 8;
    
    for (int ii = 0; ii < MAX_SEARCH; ii++)
    {
        if(!doneNeg)
        {
            float2 queryUV = searchbase - tangentUV * dNeg;
            float brightness = getluma(getsamplecolor(queryUV).xyz);
            deltaNeg = brightness - referenceLuma;
            doneNeg = abs(deltaNeg) >= stopThreshold;
        }
        if(!donePos)
        {
            float2 queryUV = searchbase + tangentUV * dPos;
            float brightness = getluma(getsamplecolor(queryUV).xyz);
            deltaPos = brightness - referenceLuma;
            donePos = abs(deltaPos) >= stopThreshold;
        }
    }
    
    if (doneNeg && donePos)
        break;
    
    if(ii+1 < MAX_SEARCH)
    {
        if (!doneNeg)
            dNeg += 1.0f;
        if(!donePos)
            dPos += 1.0f;
    }
    
    float edgeOffset = 0.5f - min(dNeg, dPos) / (dNeg + dPos);
    float nearestDelta = dNeg < dPos ? deltaNeg : deltaPos;
    
    bool validEdge = getluma(getsamplecolor(A).xyz) < 0.0f != (nearestDelta < 0.0f);
    if(!validEdge)
        edgeOffset = 0.0f;
    
    float averageluma = (2.0 * (getluma(getsamplecolor(N).xyz) + getluma(getsamplecolor(S).xyz) + getluma(getsamplecolor(W).xyz) + getluma(getsamplecolor(E).xyz))
    + +getluma(getsamplecolor(NW).xyz) + getluma(getsamplecolor(NE).xyz) + getluma(getsamplecolor(SW).xyz) + getluma(getsamplecolor(SE).xyz)) / 12.0f;
    
    float r = saturate(abs(averageluma - getluma(getsamplecolor(A).xyz) / localcontrast));
    float s = r * r * (3.0f - 2.0f * r);
    float subpixeloffset = s * s * SubPiexelStrength;
    float finaloffset = max(edgeOffset, subpixeloffset);

    float2 finalUV = input.uv + normalUV * finaloffset;
    
    return getsamplecolor(finalUV);
}
