cbuffer MatrixBuffer {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct vertexInputType {
    float4 position: POSITION;
    float4 color: COLOR;
};

struct pixelInputType {
    float4 position: SV_POSITION;
    float4 color: COLOR;
};

pixelInputType colorVertexShader(vertexInputType input) {
    pixelInputType output;
    
    input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.color = input.color;

    return output;
}
