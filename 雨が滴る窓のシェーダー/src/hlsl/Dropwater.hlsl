cbuffer cBuffer:register(b0)
{
	float2 offset:packoffset(c0.x);
	float2 addDynamicDropletPos:packoffset(c0.z);
	float2 addStaticDropletPos:packoffset(c1.x);
	float distortion : packoffset(c1.z);
	float attenuate : packoffset(c1.w);
}

Texture2D    ColorTex0 : register(t0);
Texture2D    ColorTex1 : register(t1);
Texture2D    ColorTex2 : register(t2);
SamplerState ColorSmp : register(s0);

struct VS_IN
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_OUT VS(VS_IN In)
{
	VS_OUT Out;

	Out.Position = float4(In.Position, 1.0f);
	Out.TexCoord = In.TexCoord;

	return Out;
}

struct PS_OUT//水滴の座標と軌跡のマップを同時に出力
{
	float4 Col1 : SV_TARGET0;
	float4 Col2 : SV_TARGET1;
};
//乱数生成
float GetRandomNumber(float2 texCoord)
{
	return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233))) * 43758.5453);
}

//滴マップを更新
float4 UpdateDropletsMap(float2 Tex)
{
	float4 Out = 0.0f;

	//ノイズマップのサンプリング
	float4 c1 = ColorTex2.Sample(ColorSmp, Tex + float2(-offset.x * 2.0f, 0.0f));
	float4 c2 = ColorTex2.Sample(ColorSmp, Tex + float2(-offset.x, 0.0f));
	float4 c3 = ColorTex2.Sample(ColorSmp, Tex);
	float4 c4 = ColorTex2.Sample(ColorSmp, Tex + float2(offset.x, 0.0f));
	float4 c5 = ColorTex2.Sample(ColorSmp, Tex + float2(offset.x * 2.0f, 0.0f));

	//真下へ
	if (c3.r <= c2.r && c3.r <= c4.r || c2.r == c4.r)
		Out += ColorTex0.Sample(ColorSmp, Tex + float2(0.0f, -offset.y*1.5f));

	//左へ
	if (!(c4.r <= c3.r && c4.r <= c5.r || c3.r == c5.r) && c3.r <= c5.r)
		Out += ColorTex0.Sample(ColorSmp, Tex + float2(offset.x*0.9f  , -offset.y * 0.9f));

	//右へ
	if (!(c2.r <= c1.r && c2.r <= c3.r || c1.r == c3.r) && c3.r < c1.r)
		Out += ColorTex0.Sample(ColorSmp, Tex + float2(-offset.x * 0.9f, -offset.y * 0.9f));

	return Out;
}


//滴を追加する(動的)
PS_OUT PS_AddDynamicDroplets(VS_OUT In)
{
	PS_OUT Out;

	Out.Col1 = UpdateDropletsMap(In.TexCoord);

	//滴を追加する
	if (abs(In.TexCoord.x - addDynamicDropletPos.x) <= offset.x * 0.3f &&
		abs(In.TexCoord.y - addDynamicDropletPos.y) <= offset.y * 0.3f)
	{
		Out.Col1 = float4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Out.Col2 = ColorTex1.Sample(ColorSmp, In.TexCoord) + Out.Col1 - attenuate;

	return Out;
}

//滴を追加しない(動的)
PS_OUT PS_NoneAddDynamicDroplets(VS_OUT In)
{
	PS_OUT Out;

	Out.Col1 = UpdateDropletsMap(In.TexCoord);

	Out.Col2 = ColorTex1.Sample(ColorSmp, In.TexCoord) + Out.Col1 - attenuate;

	return Out;
}

//滴を追加する（静的）
float4 PS_AddStaticDroplets(VS_OUT In) :SV_TARGET
{
	float4 Out = 0.0f;
	Out = ColorTex0.Sample(ColorSmp, In.TexCoord);

	//滴を追加する
	if (abs(In.TexCoord.x - addStaticDropletPos.x) <= offset.x/2 +offset.x * GetRandomNumber(In.TexCoord) &&
		abs(In.TexCoord.y - addStaticDropletPos.y) <= offset.y +offset.y*3 *GetRandomNumber(In.TexCoord))
	{
		Out = float4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Out-= attenuate/4;

	return Out;
}


//滴を追加しない（静的）
float4 PS_NoneAddStaticDroplets(VS_OUT In) :SV_TARGET
{
	float4 Out = 0.0f;
	Out = ColorTex0.Sample(ColorSmp, In.TexCoord);

	Out -= attenuate/4;

	return Out;
}

//波マップの静的と動的とを合成
float4 PS_MergeDroplets(VS_OUT In) :SV_TARGET
{
	float4 Out = 0.0f;
    Out += ColorTex0.Sample(ColorSmp, In.TexCoord);
    Out += ColorTex1.Sample(ColorSmp, In.TexCoord);
	return Out;
}

//ブラーを適応する
float4 PS_Blur(VS_OUT In) :SV_TARGET
{
	//テクセルを取得
	float2 Texel0 = In.TexCoord + float2(-offset.x, 0.0f);
	float2 Texel1 = In.TexCoord + float2(offset.x, 0.0f);
	float2 Texel2 = In.TexCoord + float2(0.0f,  offset.y);
	float2 Texel3 = In.TexCoord + float2(0.0f, -offset.y);

	float2 Texel4 = In.TexCoord + float2(-offset.x, -offset.y);
	float2 Texel5 = In.TexCoord + float2(offset.x, -offset.y);
	float2 Texel6 = In.TexCoord + float2(-offset.x,  offset.y);
	float2 Texel7 = In.TexCoord + float2(offset.x,  offset.y);

	//取得したテクセル位置のカラー情報を取得する。
	//それぞれのカラー値にウェイトをかけている。このウェイト値はすべての合計が 1.0f になるように調整する。
	float4 p0 = ColorTex0.Sample(ColorSmp, In.TexCoord) * 0.2f;

	float4 p1 = ColorTex0.Sample(ColorSmp, Texel0) * 0.1f;
	float4 p2 = ColorTex0.Sample(ColorSmp, Texel1) * 0.1f;
	float4 p3 = ColorTex0.Sample(ColorSmp, Texel2) * 0.1f;
	float4 p4 = ColorTex0.Sample(ColorSmp, Texel3) * 0.1f;

	float4 p5 = ColorTex0.Sample(ColorSmp, Texel4) * 0.1f;
	float4 p6 = ColorTex0.Sample(ColorSmp, Texel5) * 0.1f;
	float4 p7 = ColorTex0.Sample(ColorSmp, Texel6) * 0.1f;
	float4 p8 = ColorTex0.Sample(ColorSmp, Texel7) * 0.1f;

	//カラーを合成する
	return p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8;
}

//法線マップを作成する
float4 PS_CreateNormalMap(VS_OUT In) :SV_TARGET
{
	//上下左右のテクセル位置の高さを取得する
	float H1 = ColorTex0.Sample(ColorSmp, In.TexCoord + float2(offset.x,  0.0f)).r;
	float H2 = ColorTex0.Sample(ColorSmp, In.TexCoord + float2(0.0f,         offset.y)).r;
	float H3 = ColorTex0.Sample(ColorSmp, In.TexCoord + float2(-offset.x, 0.0f)).r;
	float H4 = ColorTex0.Sample(ColorSmp, In.TexCoord + float2(0.0f,         -offset.y)).r;

	//X 方向の高さの変化量を計算する
	//波の高さ情報は -1.0f ～ 1.0f の範囲で格納されているので 0.0f ～ 1.0f に変換する
	float tu = 0.5f * (H3 - H1) + 0.5f;

	//Y 方向の高さの変化量を計算する
	//波の高さ情報は -1.0f ～ 1.0f の範囲で格納されているので 0.0f ～ 1.0f に変換する
	float tv = 0.5f * (H4 - H2) + 0.5f;

	return float4(float3(tu, tv, 1.0f), 1.0f);
}

//バックバッファのイメージをゆがませる
float4 PS_Distortion(VS_OUT In) :SV_TARGET
{
   float4 Out = 0.0f;

   float4 offset = ColorTex0.Sample(ColorSmp, In.TexCoord);
   offset.rgb = (offset.rgb - 0.5f) * 2.0f;
   offset.r *= -1.0f;

   //滴の輪郭を取得する
   float p = 1.0f - dot(normalize(offset.rgb), float3(-0.3f, 0.0f, 1.0f));

   Out = ColorTex1.Sample(ColorSmp, In.TexCoord + offset * distortion) + p*10*distortion;

   return Out;
}
