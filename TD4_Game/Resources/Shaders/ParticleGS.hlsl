#include "ParticleShaderHeader.hlsli"

// 四角形の頂点数
static const uint vnum = 4;

// センターからのオフセットテーブル
static const float4 offset_array[vnum] =
{
    float4(-0.5f, -0.5f, 0, 0), // 左下
	float4(-0.5f, +0.5f, 0, 0), // 左上
	float4(+0.5f, -0.5f, 0, 0), // 右下
	float4(+0.5f, +0.5f, 0, 0) // 右上
};

// UVテーブル（左上が0,0　右下が1,1）
static const float2 uv_array[vnum] =
{
    float2(0, 1), // 左下
	float2(0, 0), // 左上
	float2(1, 1), // 右下
	float2(1, 0) // 右上
};

// 点の入力から、四角形を出力
[maxvertexcount(vnum)]
void main(
	point VSOutput input[1] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{
    GSOutput element;

    float4 offset = float4(0, 0, 0, 0);

    for (uint i = 0; i < vnum; i++)
    {
		// 中心からのオフセットをスケーリング
        offset = offset_array[i] * input[0].scale;
		// 中心からのオフセットをビルボード回転（モデル座標）
        offset = mul(matBill, offset);
        //さらにパーティクルの回転を適用
        offset.x = (offset.x * cos(radians(input[0].rot))) - (offset.y * sin(radians(input[0].rot)));
        offset.y = (offset.x * sin(radians(input[0].rot))) + (offset.y * cos(radians(input[0].rot)));
		// オフセット分ずらす（ワールド座標）
        element.svpos = input[0].pos + offset;
		// ビュープロジェクション変換
        element.svpos = mul(mat, element.svpos);
        element.uv = uv_array[i];
        
        //色情報はそのまま
        element.color = input[0].color;
        output.Append(element);
    }
}