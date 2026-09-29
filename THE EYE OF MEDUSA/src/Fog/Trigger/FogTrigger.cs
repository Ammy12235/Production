//=============================================================================
// <summary>
// EmittTrigger 
// </summary>
// <author>CGC_12_堀 大輔</author>
//=============================================================================
using System;
using System.Collections.Generic;
using via;
using via.attribute;
using via.physics;

namespace blackfilter
{
    /// <summary>
    ///フォグトリガークラス：パーティクル生成、リリースのトリガークラス。特定のキャラクターが入ったとき、どこのエリアをロード、アンロードするのかを指定する。
    /// </summary>
    public class FogTrigger : via.Behavior, IColliders
    {
        [DataMember] private List<GameObjectRef> loadArea;
        [DataMember] private List<GameObjectRef> unloadArea;

        /// <summary>
        /// VolumetricFogDensity
        /// </summary>
        [DataMember] bool isDensityChange = false;
        [DataMember] float destDensity = 0;
        [DataMember] float densityChangeTime = 2;


        /// <summary>
        /// VolumetricFogColor
        /// </summary>
        [DataMember] bool isVFCChange = false;
        [DataMember] Color destVFColor;
        [DataMember] float vfcChangeTime = 2;

        /// <summary>
        /// 詳細なパラメータを変更
        /// </summary>
        [DataMember] bool isADDChange = false;
        [DataMember, Slider(1.0f, 100.0f, TickFrequency = 0.05), Description("アルファ値が減少し始める距離(1~100で設定)")] float destADDistance = 0;
        [DataMember] bool isVFDFChange = false;
        [DataMember, Slider(0.0f, 1.0f, TickFrequency = 0.05),Description("VolumetricFogが濃度にどれだけ影響されるか(0~1で設定)")] float destVFDFactor = 0;
        [DataMember] bool isBFAFChange = false;
        [DataMember, Slider(0.0f, 1.0f, TickFrequency = 0.05),Description("BillboardFogが濃度にどれだけ影響されるか(0~1で設定)")] float destBFAFactor = 0;
        [DataMember] bool isUFAFChange = false;
        [DataMember, Slider(0.0f, 1.0f, TickFrequency = 0.05),Description("UnderFogが濃度にどれだけ影響されるか(0~1で設定)")] float destUFAFactor = 0;
        [DataMember] float detailParameterChangeTime = 2;

        List<FogArea> loadAreaScript = new List<FogArea>();
        List<FogArea> unloadAreaScript = new List<FogArea>();
        public override void awake()
        {
            //vec3 v = vector.lerp(vec3.One, vec3.Zero, 0);
        }

        public override void start()
        {


            for (int i = 0; i < loadArea.Count; i++)
            {
                var load_area = loadArea[i].Target;
                if (load_area != null)
                {
                    loadAreaScript.Add(loadArea[i].Target.getSameComponent<FogArea>());
                }
            }

            for (int i = 0; i < unloadArea.Count; i++)
            {
                var unload_area = unloadArea[i].Target;
                if (unload_area != null)
                {
                    unloadAreaScript.Add(unloadArea[i].Target.getSameComponent<FogArea>());
                }
            }


        }

        public override void update()
        {

        }

        public void onContact(CollisionInfo collision_info)
        {
            debug.infoLine("ContactTrigger");
            var other_collidable = collision_info.CollidableB;
            if (other_collidable.FilterInfo.Layer == via.physics.System.getLayerIndex("FogCharacter"))
            {
                //リリースが先(解放したオブジェクトを即座に次のエリアに割り当てるため)
                for (int i = 0; i < unloadAreaScript.Count; i++)
                {
                    if (unloadAreaScript[i].getState() == FogArea.FogAreaState.State_Active)
                    {
                        debug.infoLine("AreaState:ReleaseActive");
                        //マネージャに指定したエリアの資源を保存してもらう
                        FogParticleManager.Instance.ReleaseParticles(unloadArea[i].Target);
                        unloadAreaScript[i].SetState(FogArea.FogAreaState.State_NonActive);
                    }
                }

                for (int i = 0; i < loadAreaScript.Count; i++)
                {

                    if (loadAreaScript[i].getState() != FogArea.FogAreaState.State_Active)
                    {
                        debug.infoLine("AreaState:CreateActive");
                        //マネージャに生成してもらう
                        FogParticleManager.Instance.CreateParticles(loadArea[i].Target);
                        loadAreaScript[i].SetState(FogArea.FogAreaState.State_Active);
                    }

                }

                if (isDensityChange)
                {
                    FogParticleManager.Instance.SetDensity(destDensity, densityChangeTime);
                }
                if (isVFCChange)
                {
                    FogParticleManager.Instance.SetVFColor(destVFColor, vfcChangeTime);
                }
                if (isADDChange || isVFDFChange || isBFAFChange || isUFAFChange)
                {
                    FogParticleManager.Instance.SetDetailParameter(isADDChange, destADDistance,
                        isVFDFChange, destVFDFactor,
                        isBFAFChange, destBFAFactor,
                        isUFAFChange, destUFAFactor,
                        detailParameterChangeTime);
                }

                //var tag = other_collidable.GameObject.TagValue;

                    //if (tag == 2)
                    //{
                    //    //リリースが先(解放したオブジェクトを即座に次のエリアに割り当てるため)
                    //    for (int i = 0; i < unloadAreaScript.Count; i++)
                    //    {
                    //        if (unloadAreaScript[i].getState() == FogArea.FogAreaState.State_Active)
                    //        {
                    //            debug.infoLine("AreaState:ReleaseActive");
                    //            //マネージャに指定したエリアの資源を保存してもらう
                    //            FogParticleManager.Instance.ReleaseParticles(unloadArea[i].Target);
                    //            unloadAreaScript[i].SetState(FogArea.FogAreaState.State_NonActive);
                    //        }
                    //    }

                    //    for (int i = 0; i < loadAreaScript.Count; i++)
                    //    {

                    //        if (loadAreaScript[i].getState() != FogArea.FogAreaState.State_Active)
                    //        {
                    //            debug.infoLine("AreaState:CreateActive");
                    //            //マネージャに生成してもらう
                    //            FogParticleManager.Instance.CreateParticles(loadArea[i].Target);
                    //            loadAreaScript[i].SetState(FogArea.FogAreaState.State_Active);
                    //        }

                    //    }

                    //    if(isVolumetricFogDensityChange)
                    //    {
                    //        FogParticleManager.Instance.SetVolumetricFogDensity(volumetricFogDensity,2);
                    //    }

                    //}

            }
        }

        public void onOverlapping(CollisionInfo collision_info)
        {

        }

        public void onSeparate(CollisionInfo collision_info)
        {
        }
    }
}
