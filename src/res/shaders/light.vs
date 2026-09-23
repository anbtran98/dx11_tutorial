#define NUM_LIGHTS 4

cbuffer MatrixBuffer {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer {
    float3 cameraPosition;
    float padding;
};

cbuffer LightPositionBuffer {
    float4 lightPosition[NUM_LIGHTS];
};

struct VertexInput {
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInput {
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 lightPos[NUM_LIGHTS] : TEXCOORD1;
};

PixelInput LightVertexShader (VertexInput input) {
    PixelInput output;
    float4 worldPosition;

    input.position.w =  1.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = input.tex;

    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    worldPosition = mul(input.position, worldMatrix);

    for (int i = 0; i < NUM_LIGHTS; i++) {
        output.lightPos[i] = lightPosition[i].xyz - worldPosition.xyz;
        output.lightPos[i] = normalize(output.lightPos[i]);
    }
    
    // output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
    // output.viewDirection = normalize(output.viewDirection);

    return output;
}
