//=============================================================================
// <summary>
// FogParticle_BillBoard_P 
// </summary>
// <author>CGC_12_堀 大輔</author>
//=============================================================================
using System;
using System.Collections.Generic;
using via;
using via.render;
using via.attribute;
using via.effect.script;

namespace blackfilter
{
    /// <summary>
    ///ビルボードフォグパーティクルクラス：ビルボードを用いた煙パーティクルクラス。カメラの座標参照が必要。
    /// </summary>
    public class FogParticle_Billboard : FogParticle
    {

        [DataMember]
        private float rotateSpeed = 1;
        [DataMember]
        private float alphaDistance = 1;

       
        private float moveYInitPos;
        private float timer;

        private Mesh fogMesh;
        
        private MaterialParam param0;
        private MaterialParam param1;
        private MaterialParam param2;
        public override void awake()
        {
            moveYInitPos = random.genF32()*2*MathF.PI;
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
                debug.errorLine("FogParticle_Billboard:Meshが取得できません");
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
            timer += 0.016f;
            GameObject.Transform.Position = new vec3(position.x, position.y+MathF.Sin(moveYInitPos + timer)/2, position.z);
            //向きの更新
            LookAt(targetObj.Transform.Position);

        }

        private void LookAt(vec3 targetPos)
        {

            vec3 dir = targetPos - this.GameObject.Transform.Position;


            rotation.z = 0;
            rotation.y = MathEx.normalizeRad(math.atan2(camera.GameObject.Transform.AxisZ.x, camera.GameObject.Transform.AxisZ.z));

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

            this.GameObject.Transform.Rotation = qua;
        }

        public void SetActiveMesh(bool isActive)
        {
            if(fogMesh!=null)
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
