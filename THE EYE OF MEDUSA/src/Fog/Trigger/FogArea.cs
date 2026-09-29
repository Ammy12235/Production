//=============================================================================
// <summary>
// FogArea 
// </summary>
// <author>CGC_12_堀 大輔</author>
//=============================================================================
using System.Collections.Generic;
using via;
using via.attribute;
using via.physics;


namespace blackfilter
{
    /// <summary>
    ///フォグエリアクラス：実際のパーティクルを管理し、更新するクラス。生成でいえばどこにいくつ生成するか、開放でいえば何をいくつ開放するのかを決めるまとまりとして機能する。
    /// </summary>
    [UpdateOrder((int)UpdateOrder.FogArea)]
    public class FogArea : via.Behavior, IColliders
    {
        [DataMember, Slider(0.0f, 100.0f, TickFrequency = 1)] private float density = 50.0f;

        private LinkedList<FogParticle_Billboard> particleBillboardScripts = new LinkedList<FogParticle_Billboard>();
        private LinkedList<FogParticle_Under> particleUnderScripts = new LinkedList<FogParticle_Under>();

        private List<GameObject> gasCharacterObjs = new List<GameObject>();
        private List<FogForceField> forceFields = new();
        private List<vec3> prevPos = new();
        private List<int> gasCharacterHash = new List<int>();

        private GameObject targetObj;//カメラを格納
        private Camera camera;
        public enum FogAreaState
        {
            State_None = -1,
            State_Active,
            State_NonActive,
            State_Max
        }

        FogAreaState state = FogAreaState.State_NonActive;
        public override void awake()
        {
        }

        public override void start()
        {
            state = FogAreaState.State_NonActive;
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
        }

        public override void onDestroy()
        {
            base.onDestroy();

            if (FogParticleManager.isValid())
            {
                FogParticleManager.Instance.ReleaseParticles(GameObject);
            }
        }

        public void UpdateArea(FogParticleManager.particleParameter parameter)
        {
            //エリアの更新。エリアがアクティブでなかったら更新しない。
            if (state != FogAreaState.State_Active) return;

            //float distance = vector.distance(this.GameObject.Transform.Position, targetObj.Transform.Position);
            //if (distance > parameter.cParam.areaCullingDistance)
            //{
            //    if (state == FogAreaState.State_Active)
            //    {
            //        debug.infoLine("AreaState:ReleaseActive");
            //        //マネージャに指定したエリアの資源を保存してもらう
            //        FogParticleManager.Instance.ReleaseParticles(this.GameObject);
            //        SetState(FogAreaState.State_NonActive);
            //    }
            //}
            //else
            //{
            //    if (state == FogAreaState.State_NonActive)
            //    {
            //        debug.infoLine("AreaState:CreateActive");
            //        //マネージャに指定したエリアの資源を保存してもらう
            //        FogParticleManager.Instance.CreateParticles(this.GameObject);
            //        SetState(FogAreaState.State_Active);
            //    }
            //}
            updateParticleParameter(parameter);

        }

        //==================================================
        //マネージャクラスから受け取ったスクリプトをセットしていく
        //==================================================
        public void SetUnderFogScript(FogParticle_Under script, vec3 initPos)
        {

            particleUnderScripts.AddLast(script);
            script.SetInitPos(initPos);
            script.SetActiveMesh(true);
        }

        public void SetBillboardFogScript(FogParticle_Billboard script, vec3 initPos)
        {
            particleBillboardScripts.AddLast(script);
            script.SetInitPos(initPos);
            script.SetActiveMesh(true);
        }

       

        //==================================================
        //マネージャクラスへスクリプトをリリースしていく
        //==================================================
        public LinkedList<FogParticle_Under> ReleaseUnderFogScript()
        {
            return particleUnderScripts;
        }

        public LinkedList<FogParticle_Billboard> ReleaseBillboardFogScript()
        {
            return particleBillboardScripts;
        }

        public void SetState(FogAreaState areaState)
        {
            state = areaState;
        }

        public FogAreaState getState()
        {
            return state;
        }

        void updateParticleParameter(FogParticleManager.particleParameter parameter)
        {
            float distance;
            
            if (particleUnderScripts == null)
            {
                return;
            }

            foreach (var uLinkedList in particleUnderScripts)
            {
                //キャラクターがゼロの場合
                if (gasCharacterObjs.Count == 0)
                {
                    goto Exit;//キャラクターとのインタラクションをスキップし、更新のみにする
                }
                else//キャラクターがいた場合
                {
                    for (int i = 0; i < gasCharacterObjs.Count; i++)
                    {
                        //倒されたなどの理由でオブジェクトが見つからなかったら削除
                        if (gasCharacterObjs[i] == null)
                        {
                            gasCharacterObjs.Remove(gasCharacterObjs[i]);
                            // nullだったら下の処理は例外が発生する
                            prevPos.RemoveAt(i);
                            forceFields.RemoveAt(i);
                            continue;
                        }
                        //速度計算
                        vec3 velocity = gasCharacterObjs[i].Transform.Position - prevPos[i];
                        distance = uLinkedList.CulcVelocity(this, gasCharacterObjs[i].Transform.Position, vec3.One, forceFields[i].GetRadius(), forceFields[i].GetPower(), parameter.uParam.weight);
                    }
                }
                //カメラからの距離に応じてカリングするかを決定
                //描画しないのならば更新しない
                if (uLinkedList.SetIsCulling(targetObj.Transform.Position, parameter.uParam.cullingDistance) == false) continue;

                Exit:
                //各種パラメータをセット
                uLinkedList.SetDensity(density * parameter.uParam.alphaFactor / 100);
                uLinkedList.SetScale(parameter.uParam.scale);
                uLinkedList.SetHeight(parameter.uParam.height);
                uLinkedList.SetRotateSpeed(parameter.uParam.rotateSpeed);
                uLinkedList.SetSoftParticleThreshold(parameter.cParam.softParticleThresHold);
                uLinkedList.SetAlphaDistance(parameter.cParam.alphaDecrementStartDistance);

                //更新
                uLinkedList.UpdateParticle();
            }

            foreach (var bLinkedList in particleBillboardScripts)
            {
                //キャラクターがゼロの場合
                if (gasCharacterObjs.Count == 0)
                {
                    goto Exit;//キャラクターとのインタラクションをスキップし、更新のみにする
                }
                else//キャラクターがいた場合
                {
                    for (int i = 0; i < gasCharacterObjs.Count; i++)
                    {
                        //倒されたなどの理由でオブジェクトが見つからなかったら削除
                        if (gasCharacterObjs[i] == null)
                        {
                            gasCharacterObjs.Remove(gasCharacterObjs[i]);
                            // nullだったら下の処理は例外が発生する
                            prevPos.RemoveAt(i);
                            forceFields.RemoveAt(i);
                            continue;
                        }
                        //速度計算
                        vec3 velocity= gasCharacterObjs[i].Transform.Position - prevPos[i];
                        distance = bLinkedList.CulcVelocity(this, gasCharacterObjs[i].Transform.Position, vec3.One, forceFields[i].GetRadius(), forceFields[i].GetPower(), parameter.bParam.weight);

                        //今の位置を保存
                        prevPos[i] = gasCharacterObjs[i].Transform.Position;
                    }
                }
                //描画しないのならば更新しない
                if (bLinkedList.SetIsCulling(targetObj.Transform.Position, parameter.bParam.cullingDistance) == false) continue;

                Exit:
                //各種パラメータをセット
                bLinkedList.SetDensity(density * parameter.bParam.alphaFactor / 100);
                bLinkedList.SetScale(parameter.bParam.scale);
                bLinkedList.SetSoftParticleThreshold(parameter.cParam.softParticleThresHold);
                bLinkedList.SetAlphaDistance(parameter.cParam.alphaDecrementStartDistance);

                //更新
                bLinkedList.UpdateParticle();

            }


        }

        public void onContact(CollisionInfo collision_info)
        {
            var other_collidable = collision_info.CollidableB;
            if (other_collidable.FilterInfo.Layer == via.physics.System.getLayerIndex("FogCharacter"))
            {
                gasCharacterObjs.Add(other_collidable.GameObject);

                FogForceField field = other_collidable.GameObject.getComponent<FogForceField>();
                if (field == null)
                {
                    debug.errorLine("FogCharacterを設定したオブジェクトにはFogForceFieldコンポーネントをつけてください");
                }
                else forceFields.Add(field);

                prevPos.Add(other_collidable.GameObject.Transform.Position);
                gasCharacterHash.Add(other_collidable.GetHashCode());
                debug.infoLine("Area:Player Detected");
                //var tag = other_collidable.GameObject.TagValue;


            }
        }

        public void onOverlapping(CollisionInfo collision_info)
        {

        }

        public void onSeparate(CollisionInfo collision_info)
        {
            var other_collidable = collision_info.CollidableB;
            for (int i = 0; i < gasCharacterHash.Count; i++)
            {
                if (gasCharacterHash[i] == other_collidable.GetHashCode())
                {
                    var other_obj = other_collidable.GameObject;

                    gasCharacterHash.Remove(collision_info.GetHashCode());
                    if (other_obj != null)
                    {
                        prevPos.Remove(other_collidable.GameObject.Transform.Position);
                        gasCharacterObjs.Remove(other_collidable.GameObject);
                        forceFields.Remove(other_collidable.GameObject.getComponent<FogForceField>());
                    }
                    debug.infoLine("Area:Character Separeted");
                    break;
                }
            }
        }
    }
}
