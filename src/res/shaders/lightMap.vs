cbuffer MatrixBuffer {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInput {
    float4 position: POSITION;
    float2 tex: TEXCOORD0;
    float3 normal: NORMAL;
};

struct PixelInput {
    float4 position : SV_POSITION;
    float2 tex:TEXCOORD0;
};

PixelInput LightMapVertexShader(VertexInput input) {
    PixelInput output;
    
    input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = input.tex;

    return output;
}

