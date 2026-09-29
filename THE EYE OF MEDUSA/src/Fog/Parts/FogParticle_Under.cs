//=============================================================================
// <summary>
// FogParticle_Under_P 
// </summary>
// <author>CGC_12_堀 大輔</author>
//=============================================================================

using via;
using via.attribute;
using via.effect.script;
using via.render;

namespace blackfilter
{
    /// <summary>
    ///アンダーフォグパーティクルクラス：地面の煙パーティクルクラス。高さと回転速度の指定が必要。
    /// </summary>
    public class FogParticle_Under : FogParticle
    {
        [DataMember]
        private float rotateSpeed = 1;
        [DataMember]
        private float alphaDistance = 1;

        private Mesh fogMesh;

        private MaterialParam param0;
        private MaterialParam param1;
        private MaterialParam param2;
        public override void awake()
        {
        }

        public override void start()
        {
            //初期位置を設定
            base.start();

            // MaterialsのIndexでアクセスします
            int materialIndex = 0;
            fogMesh = GameObject.getComponent<Mesh>();
            if (fogMesh == null)
            {
                debug.errorLine("FogParticle_Under:Meshが取得できません");
            }
            param0 = fogMesh.Materials[materialIndex];
            materialIndex++;
            param1 = fogMesh.Materials[materialIndex];
            materialIndex++;
            param2 = fogMesh.Materials[materialIndex];
        }

        public void UpdateParticle()
        {
            //位置とスケールを更新。
            UpdatePos();
            LookAt(this.GameObject.Transform.Position + vec3.AxisY);
            rotation.y += rotateSpeed;
            GameObject.Transform.Rotation = quaternion.makeRotateXYZ(rotation);
        }

        protected void LookAt(vec3 targetPos)
        {

            vec3 dir = targetPos - this.GameObject.Transform.Position;


            rotation.z = 0;
            rotation.y = MathEx.normalizeRad(math.atan2(dir.x, dir.z));

            //X軸回転を計算
            vec3 xz_dir = dir;
            xz_dir.y = 0.0f;
            float xz_length = vector.length(xz_dir);

            if (math.abs(dir.z) < math.Epsilon)
            {
                rotation.x = math.PIDIV2;
            }
            else
            {
                rotation.x = math.atan2(dir.y, dir.z * (xz_length / math.abs(dir.z)));
            }

            //Y軸方向は上に向かせる
            if (rotation.x > (math.PIDIV2))
            {
                rotation.x = math.PI - rotation.x;
            }
            else if (rotation.x < (-math.PIDIV2))
            {
                rotation.x = -math.PI + rotation.x;
            }

            if (dir.y < 0.0f)
            {
                if ((-math.PIDIV2 < rotation.y) && (rotation.y < math.PIDIV2))
                {
                    rotation.x *= -1.0f;
                }
            }
            else
            {
                rotation.x *= -1.0f;
            }
            Quaternion qua = quaternion.makeRotateXYZ(rotation);

            GameObject.Transform.Rotation = qua;
        }

        public void SetActiveMesh(bool isActive)
        {
            if (fogMesh != null)
                fogMesh.Enabled = isActive;
        }

        public bool SetIsCulling(vec3 cameraPos, float cullingDistance)
        {
            vec3 particlePos = GameObject.Transform.Position;
            float distance = math.sqrtf(math.pow(cameraPos.x - particlePos.x, 2) + math.pow(cameraPos.z - particlePos.z, 2));

            if (distance > cullingDistance)
            {
                SetActiveMesh(false);
                return false;
            }
            else
            {
                SetActiveMesh(true);
                return true;
            }

        }
        public void SetHeight(float h)
        {
            position.y = initPosition.y+ h;
        }

        public void SetDensity(float density)
        {
            fogDensity = density;
            param0.ValueF = fogDensity;
        }

        public void SetSoftParticleThreshold(float threshold)
        {
            softParticleThreshold = threshold;
            param1.ValueF = softParticleThreshold;
        }

        public void SetScale(float scale)
        {
            localScale = new vec3(scale, scale, scale);
        }

        public void SetRotateSpeed(float speed)
        {
            rotateSpeed = speed;
        }


        public void SetAlphaDistance(float distance)
        {
            alphaDistance = distance;
            param2.ValueF = alphaDistance;
        }

    }
}
