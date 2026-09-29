//=============================================================================
// <summary>
// FogParticle 
// </summary>
// <author>CGC_12_堀 大輔</author>
//=============================================================================
using System;
using System.Collections.Generic;
using via;
using via.attribute;
using via.effect.script;
using via.render;

namespace blackfilter
{
    /// <summary>
    ///フォグパーティクルクラス：すべての煙パーティクルの基底クラス。Setter、計算関数の提供や位置更新を担う。
    /// </summary>
    public class FogParticle : via.Behavior
    {

        [IgnoreDataMember, ManipulatorInspectBtn]
        protected vec3 position = new vec3(0.0f, 1.0f, 1.0f);
        protected vec3 initPosition = new vec3(0.0f, 1.0f, 1.0f);
        [IgnoreDataMember, ManipulatorInspectBtn]
        protected vec3 rotation = new vec3(0.0f, 1.0f, 0.0f);
        [IgnoreDataMember, ManipulatorInspectBtn]
        protected vec3 localScale = new vec3(0.0f, 1.0f, 1.0f);
        [IgnoreDataMember]
        protected vec3 velocity = new vec3(0.0f, 0.0f, 0.0f);

        protected float fogDensity = 0;
        protected float softParticleThreshold = 0;

        protected GameObject targetObj;//カメラを格納
        protected Camera camera;

        public override void awake()
        {

        }

        //初期座標の格納
        public override void start()
        {
            targetObj = CameraManager.Instance.GameObject;
            if (targetObj == null)
            {
                debug.errorLine("FogParticle:Cameraが取得できません");
            }
            //camera = CameraManager_P.Instance.getCpCamera();
            camera = targetObj.getSameComponent<Camera>();
            if (camera == null)
            {
                debug.errorLine("FogParticle:Cameraが取得できません");
            }

            GameObject.Transform.Position = new vec3(position.x, position.y, position.z);
            initPosition = GameObject.Transform.Position;

        }

        //位置とスケールを更新。
        public void UpdatePos()
        {
            //インタラクトの結果発生した速度
            position += velocity;

            //フォグ自身が元の一に戻ろうとする力
            position += (initPosition - position) / 300;

            GameObject.Transform.Position = new vec3(position.x, position.y, position.z);
            GameObject.Transform.LocalScale = new vec3(localScale.x, localScale.y, localScale.z);
        }

        public void SetInitPos(vec3 pos)
        {
            initPosition = pos;
            position = initPosition;
            GameObject.Transform.Position = new vec3(position.x, position.y, position.z);
        }

        public float CulcVelocity(FogArea area, vec3 emittionPos, vec3 emittionVelocity, float emittionRadius, float power, float weight)
        {
            vec3 particlePos = GameObject.Transform.Position;
            vec3 areaPos = area.GameObject.Transform.Position;
            vec3 areaScale = area.GameObject.Transform.LocalScale;

            float distance = math.sqrtf(math.pow(emittionPos.x - particlePos.x, 2) + math.pow(emittionPos.z - particlePos.z, 2));

            //もしエリアの外側であれば速度を加算しない
            if (particlePos.x > areaPos.x + areaScale.x)
            {
                GameObject.Transform.Position = particlePos - new vec3(0.01f, 0, 0);
                velocity = vec3.Zero;
                return distance;
            }
            else if (particlePos.x < areaPos.x - areaScale.x)
            {
                GameObject.Transform.Position = particlePos + new vec3(0.01f, 0, 0);
                velocity = vec3.Zero;
                return distance;
            }

            if (particlePos.z > areaPos.z + areaScale.z)
            {
                GameObject.Transform.Position = particlePos - new vec3(0, 0, 0.01f);
                velocity = vec3.Zero;
                return distance;

            }
            else if (particlePos.z < areaPos.z - areaScale.z)
            {
                GameObject.Transform.Position = particlePos + new vec3(0, 0, 0.01f);
                velocity = vec3.Zero;
                return distance;
            }

            if (emittionVelocity.length() > 0.001f)
            {
                if (distance < emittionRadius)
                {
                   
                    float vecAngle = math.atan2(particlePos.z - emittionPos.z, particlePos.x - emittionPos.x);
                    velocity.x += math.cos(vecAngle) * power / weight;
                    velocity.z += math.sin(vecAngle) * power / weight;
                }
                else
                {
                    velocity *= 0.99f;
                }
            }
            else return distance;

            return distance;

        }


    }
}
