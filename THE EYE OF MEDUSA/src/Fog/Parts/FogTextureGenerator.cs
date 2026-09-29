//=============================================================================
// <summary>
// FogTextureGenerator_P 
// </summary>
// <author>CGC_12_堀 大輔</author>
//=============================================================================

using via;
using via.attribute;
using via.render;

namespace blackfilter
{
    /// <summary>
    ///フォグテクスチャジェネレータ：テクスチャを複数組み合わせてフォグテクスチャを作成するクラス。
    /// </summary>
    public class FogTextureGenerator : via.Behavior
    {
        [DataMember] private int maxParticleNum = 30;
        [DataMember] private float obbScale = 0.2f;
        [DataMember] private float rotateSpeed = 0.005f;
       

        Stamp stamp;
        OBB obbs;
        vec3 initPos;
        //テクスチャ上のテクスチャのパラメータ
        struct textureParameter
        {
            public vec2 pos;
            public vec2 vel;
            public float rot;
            public float rotTotal;
        }

        private textureParameter[] texParameters = new textureParameter[32];

        public override void awake()
        {
        }

        public override void start()
        {
            initPos = GameObject.Transform.Position;

            
            stamp = GameObject.getComponent<Stamp>();
            for (int i = 0; i < maxParticleNum; i++)
            {
                texParameters[i].pos.x = initPos.x + random.genF32() % 2 - 0.5f;
                texParameters[i].pos.y = initPos.z + random.genF32() % 2 - 0.5f;
                texParameters[i].vel.x = 0;
                texParameters[i].vel.y = 0;
                texParameters[i].rot = random.genU32() % 10 * rotateSpeed;
                if (texParameters[i].rot > 5) texParameters[i].rot *= -1;

            }
        }

        public override void update()
        {
            //レンダーターゲットテクスチャをクリア
            stamp.clearTexture();

            //描画する範囲を指定するOBBのTransformを決定し、描画のリクエストを送る
            for (int i = 0; i < maxParticleNum; i++)
            {

                obbs.Position = new vec3(texParameters[i].pos.x, GameObject.Transform.Position.y, texParameters[i].pos.y);
                obbs.RotateAngle = new vec3(0, texParameters[i].rotTotal, 0);
                obbs.extent = new vec3(obbScale, 10, obbScale);
                texParameters[i].rotTotal += rotateSpeed * texParameters[i].rot;


                OBB fetchObb;
                fetchObb = obbs.applyCoordScaleToExtent();

                stamp.requestDraw(fetchObb, 0, true);
            }

            //this.GameObject.Transform.Position = initPos;

        }
    }
}
