//=============================================================================
// <summary>
// FogParticle 
// </summary>
// <author>CGC_12_堀 大輔</author>
//=============================================================================

using System;
using System.Collections.Generic;
using app;
using via;
using via.attribute;
using via.render;

namespace blackfilter
{
    /// <summary>
    ///フォグパーティクルマネージャークラス：煙表現のパラメータ及び生成機能を持ったクラス。ガスのパラメータ構造やプールオブジェクトの管理もする。
    /// </summary>
    [UpdateOrder((int)UpdateOrder.FogParticleManager)]
    public class FogParticleManager : Singleton<FogParticleManager>
    {
        [DataMember, Description("テスト用か")] bool isTest = false;

        [DataMember, Slider(0.0f, 100.0f, TickFrequency = 1)] private float density = 0.0f;

        [DataMember, Description("アンダーフォグの生成の総数")] int particleUnderNum = 600;
        [DataMember, Description("ビルボードーフォグの生成の総数")] int particleBillboardNum = 600;
        [DataMember, Description("エミッターフォグの生成の総数")] int particleEmitterNum = 60;

        [DataMember, Description("X軸の生成範囲")] private float AreaScaleX = 20;
        [DataMember, Description("Z軸の生成範囲")] private float AreaScaleZ = 20;

        [DataMember, Slider(0.0f, 3.0f, TickFrequency = 0.05),
            Description("アンダーフォグ生成時どれだけ上下にばらけさせるか")]
        private float underFogRandomFactor;
        [DataMember, Slider(0.0f, 3.0f, TickFrequency = 0.05),
            Description("ビルボードフォグ生成時どれだけ上下にばらけさせるか")]
        private float billboardFogRandomFactor;

        [DataMember, Description("パーティクルを格納するファイルのパス")] private string fogFilePath;


        [DataMember] private Prefab underFogParticle = null;

        [DataMember, Description("スケール")] private float underFogScale = 1;
        [DataMember, Description("回転速度")] private float underFogRotateSpeed = 1;
        [DataMember, Description("重さ")] private float underFogWeight = 10;
        [DataMember, Description("高さオフセット")] private float underFogHeight = 1;
        [DataMember, Slider(0.0f, 1.0f, TickFrequency = 0.05), Description("濃度に対してのアルファ値の比率")] private float underFogAlphaFactor = 1;
        [DataMember, Description("カリング距離")] private float underFogCullingDistance = 5;

        [DataMember] private Prefab billboardFogParticle = null;

        [DataMember, Description("スケール")] private float billboardFogScale = 1;
        [DataMember, Description("回転速度(機能なし)")] private float billboardFogRotateSpeed = 1;
        [DataMember, Description("重さ")] private float billboardFogWeight = 10;
        [DataMember, Slider(0.0f, 1.0f, TickFrequency = 0.05), Description("濃度に対してのアルファ値の比率")] private float billboardFogAlphaFactor = 1;
        [DataMember, Description("カリング距離")] private float billboardFogCullingDistance = 5;

        [DataMember] private GameObjectRef hudFog;
        GameObject hudObj;
        private HUDFogController hudFogController;
        [DataMember, Slider(0.0f, 10.0f, TickFrequency = 0.05), Description("HUDフォグが濃度に対してどれだけ影響を受けるか")] private float hudFogFactor = 1;
        //[DataMember] private float hudFogRotateSpeed = 1;

        [DataMember] private Prefab emitterFogParticle;


        [DataMember, Slider(0.0f, 1.0f, TickFrequency = 0.05), Description("volumerticFogの濃度に対してのアルファ値の比率")] private float volumetricDensityFactor = 0.1f;
        [DataMember, Slider(1, 100.0f, TickFrequency = 0.05), Description("アルファ値減少開始距離（指定距離から線形に減少）")] private float alphaDecrementStartDistance = 10.0f;
        [DataMember, Slider(0.0f, 1.0f, TickFrequency = 0.05), Description("ソフトパーティクルの閾値")] private float softParticleThreshold = 0.02f;
        [DataMember, Slider(0.0f, 100.0f, TickFrequency = 0.05), Description("エリアのカリング距離")] private float areaCullingDistance = 10.0f;




        //[DataMember] private Material fogMaterial;

        private Stack<FogParticle_Billboard> particleBillboardScripts = new Stack<FogParticle_Billboard>();
        private Stack<FogParticle_Under> particleUnderScripts = new Stack<FogParticle_Under>();



        private List<FogParticle_Billboard> testParticleBillboardScripts = new List<FogParticle_Billboard>();
        private List<FogParticle_Under> testParticleUnderScripts = new List<FogParticle_Under>();


        bool isInitEnd = false;
        VolumetricFog volumetricFog = null;
        Folder stageFolder;
        bool initParticle = false;
        bool isTestAwake = false;

        //パラメータ変更用変数
        bool isChangeDensity = false;
        bool isChangeColor;


        float srcDensity = 0;
        float destDensity = 0;

        bool isChangeAdd = false;
        float srcAdd;
        float destAdd;

        bool isChangeVfdf = false;
        float srcVfdf;
        float destVfdf;

        bool isChangeBfaf = false;
        float srcBfaf;
        float destBfaf;

        bool isChangeUfaf = false;
        float srcUfaf;
        float destUfaf;

        vec3 destRGB;
        vec3 srcRGB;
        Color destVFColor;

        float vfdChangeTime = 0;
        float vfcChangeTime = 0;
        float dpChangeTime = 0;

        float dTimer = 0;
        float dpTimer = 0;
        float cTimer = 0;



        FogTextureGenerator textureGen;
        FogAreaContainer areaContainer;
        //private FogObjectPooler objectPooler_Under=new FogObjectPooler(create, 1000);
        public struct particleParameter
        {
            public underParameter uParam;
            public billboardParameter bParam;
            public commonParmeter cParam;

        }

        public struct underParameter
        {
            public float scale;
            public float rotateSpeed;
            public float weight;
            public float height;
            public float alphaFactor;
            public float cullingDistance;
        }

        public struct billboardParameter
        {
            public float scale;
            public float rotateSpeed;
            public float weight;
            public float alphaFactor;
            public float cullingDistance;

        }

        public struct commonParmeter
        {
            public float density;
            public float alphaDecrementStartDistance;
            public float softParticleThresHold;
            public float areaCullingDistance;
        }

        particleParameter pParameter;

        public enum eFogParticle
        {
            Particle_None = -1,
            particle_Under,
            particle_Billboard,
            particle_Emitter,
            particle_HUD,
            particle_Max,
        }



        public override void awake()
        {
            isTestAwake = isTest;
        }

        public override void start()
        {
            volumetricFog = GameObject.getComponent<VolumetricFog>();
            if (volumetricFog == null)
            {
                via.debug.errorLine("FogParticleManager:VolumetricFogが取得できません");
                return;
            }
            stageFolder = SceneManager.MainScene.findFolder(fogFilePath);
            if (stageFolder == null)
            {
                via.debug.errorLine("FogParticleManager:フォグを格納するフォルダが取得できません");
                return;
            }
            areaContainer = GameObject.getComponent<FogAreaContainer>();
            if (areaContainer == null)
            {
                via.debug.errorLine("FogParticleManager:エリアコンテナが取得できません");
                return;
            }
            hudObj = hudFog.Target;
            hudFogController = hudObj.getComponent<HUDFogController>();
            if (areaContainer == null)
            {
                via.debug.errorLine("FogParticleManager:HUDFogControllerが取得できません");
                return;
            }
            areaContainer.InitContainer();




            if (isTestAwake)
                instantiateTestParticle();
        }
        public override void update()
        {
            //パラメータを更新する
            /*
             */
            if (isTestAwake)
                updateTestParticleParameter();
            else
            {
                setParameter(ref pParameter);//マネージャーのパラメータ変数をセット　
                areaContainer.UpdateContainer(pParameter);//コンテナの更新とパラメータ送信


                if (isChangeDensity)
                {
                    lerpDensity(destDensity, vfdChangeTime);
                }
                if (isChangeColor)
                {
                    lerpColor(destVFColor, vfcChangeTime);
                }
                if (isChangeAdd || isChangeVfdf || isChangeBfaf || isChangeUfaf)
                {
                    lerpDetailParameter(dpChangeTime);
                }
                volumetricFog.Density = density * volumetricDensityFactor / 100;
                hudFogController.SetPanelColor(volumetricFog.Color, volumetricFog.Density * hudFogFactor);

            }
        }


        void lerpDensity(float destDensity, float time)
        {
            dTimer += Application.DeltaTime / Application.BaseFps;
            if (dTimer >= time)
            {
                dTimer = time;
            }
            density = time > math.Epsilon
                ? math.lerp(srcDensity, destDensity, dTimer / time)
                : destDensity;

            if (dTimer == time)
            {
                dTimer = 0.0f;
                isChangeDensity = false;
            }
        }

        void lerpDetailParameter(float time)
        {
            dpTimer += Application.DeltaTime / Application.BaseFps;
            if (dpTimer >= time)
            {
                dpTimer = time;
            }

            if (isChangeAdd)
            {
                alphaDecrementStartDistance = math.lerp(srcAdd, destAdd, dpTimer / time);
            }
            if (isChangeVfdf)
            {
                volumetricDensityFactor = math.lerp(srcVfdf, destVfdf, dpTimer / time);
            }
            if (isChangeBfaf)
            {
                billboardFogAlphaFactor = math.lerp(srcBfaf, destBfaf, dpTimer / time);

            }
            if (isChangeUfaf)
            {
                underFogAlphaFactor = math.lerp(srcUfaf, destUfaf, dpTimer / time);
            }

            if (dpTimer == time)
            {
                dpTimer = 0.0f;
                isChangeAdd = false; isChangeVfdf = false; isChangeBfaf = false; isChangeUfaf = false;
            }
        }

        void lerpColor(Color color, float time)
        {
            cTimer += Application.DeltaTime / Application.BaseFps;
            if (cTimer >= time)
            {
                cTimer = time;
            }
            vec3 RGB = vector.lerp(srcRGB, destRGB, cTimer / time);
            volumetricFog.Color = new Color((int)RGB.x, (int)RGB.y, (int)RGB.z, 1);
            if (cTimer == time)
            {
                cTimer = 0.0f;
                isChangeColor = false;
            }

        }
        public void SetDensity(float d, float time)
        {
            dTimer = 0.0f;
            destDensity = d;
            srcDensity = density;
            vfdChangeTime = time;
            isChangeDensity = true;
        }

        public void SetDetailParameter(
            bool isAdd, float add,
            bool isVfdf, float vfdf,
            bool isBfaf, float bfaf,
            bool isUfaf, float ufaf,
            float time)
        {
            isChangeAdd = false; isChangeVfdf = false; isChangeBfaf = false; isChangeUfaf = false;


            dpTimer = 0;
            isChangeAdd = isAdd;
            destAdd = add;
            srcAdd = alphaDecrementStartDistance;

            isChangeVfdf = isVfdf;
            destVfdf = vfdf;
            srcVfdf = volumetricDensityFactor;

            isChangeBfaf = isBfaf;
            destBfaf = bfaf;
            srcBfaf = billboardFogAlphaFactor;

            isChangeUfaf = isUfaf;
            destUfaf = ufaf;
            srcUfaf = underFogAlphaFactor;

            dpChangeTime = time;
        }

        public void SetVFColor(Color color, float time)
        {
            cTimer = 0.0f;
            destRGB = new vec3(color.r, color.g, color.b);
            Color srcVFColor = volumetricFog.Color;
            srcRGB = new vec3(srcVFColor.r, srcVFColor.g, srcVFColor.b);
            vfcChangeTime = time;
            isChangeColor = true;
        }
        void setParameter(ref particleParameter parameter)
        {

            parameter.uParam.scale = underFogScale;
            parameter.uParam.rotateSpeed = underFogRotateSpeed;
            parameter.uParam.weight = underFogWeight;
            parameter.uParam.height = underFogHeight;
            parameter.uParam.alphaFactor = underFogAlphaFactor;
            parameter.uParam.cullingDistance = underFogCullingDistance;

            parameter.bParam.scale = billboardFogScale;
            parameter.bParam.rotateSpeed = billboardFogRotateSpeed;
            parameter.bParam.weight = billboardFogWeight;
            parameter.bParam.alphaFactor = billboardFogAlphaFactor;
            parameter.bParam.cullingDistance = billboardFogCullingDistance;

            parameter.cParam.density = density;
            parameter.cParam.alphaDecrementStartDistance = alphaDecrementStartDistance;
            parameter.cParam.softParticleThresHold = softParticleThreshold;
            parameter.cParam.areaCullingDistance = areaCullingDistance;
        }

        void instantiateTestParticle()
        {

            vec3 centerPos = GameObject.Transform.Position;
            for (int i = 0; i < particleUnderNum; i++)
            {

                if (underFogParticle == null)
                {
                    via.debug.errorLine("FogParticleManager:underFogParticleが設定されていません");
                    return;
                }

                vec3 pos = getTestParticleInitPos(centerPos, 1, 1, underFogRandomFactor);
                GameObject particle = underFogParticle.instantiate(pos, stageFolder);
                FogParticle_Under script = particle.getComponent<FogParticle_Under>();

                testParticleUnderScripts.Add(script);
                script.SetInitPos(pos);
            }

            for (int i = 0; i < particleBillboardNum; i++)
            {

                if (billboardFogParticle == null)
                {
                    via.debug.errorLine("FogParticleManager:billboardFogParticleが設定されていません");
                    return;
                }
                vec3 pos = getTestParticleInitPos(centerPos, 1, 1, billboardFogRandomFactor);
                GameObject particle = billboardFogParticle.instantiate(pos, stageFolder);
                FogParticle_Billboard script = particle.getComponent<FogParticle_Billboard>();

                testParticleBillboardScripts.Add(script);
                script.SetInitPos(pos);
            }
        }
        void updateTestParticleParameter()
        {
            volumetricFog.Density = density * volumetricDensityFactor / 100;

            //float distance;
            for (int i = 0; i < particleUnderNum; i++)
            {
                //速度計算、濃度、大きさ、高さ、回転速度、閾値
                //distance = testParticleUnderScripts[i].CulcVelocity(vec3.Zero, vec3.Zero, emittRadius, power / 1000, underFogWeight);
                if (testParticleUnderScripts[i].SetIsCulling(vec3.Zero, underFogCullingDistance) == false) continue;

                testParticleUnderScripts[i].SetDensity(density * underFogAlphaFactor / 100);
                testParticleUnderScripts[i].SetScale(underFogScale);
                testParticleUnderScripts[i].SetHeight(underFogHeight);
                testParticleUnderScripts[i].SetRotateSpeed(underFogRotateSpeed);
                testParticleUnderScripts[i].SetSoftParticleThreshold(softParticleThreshold);
                testParticleUnderScripts[i].SetAlphaDistance(alphaDecrementStartDistance);

                testParticleUnderScripts[i].UpdateParticle();
            }

            for (int i = 0; i < particleBillboardNum; i++)
            {
                //速度計算、濃度、大きさ、閾値
                //distance = testParticleBillboardScripts[i].CulcVelocity(vec3.Zero, vec3.Zero, emittRadius, power / 1000, billboardFogWeight);
                if (testParticleBillboardScripts[i].SetIsCulling(vec3.Zero, billboardFogCullingDistance) == false) continue;

                testParticleBillboardScripts[i].SetDensity(density * billboardFogAlphaFactor / 100);
                testParticleBillboardScripts[i].SetScale(billboardFogScale);
                testParticleBillboardScripts[i].SetSoftParticleThreshold(softParticleThreshold);
                testParticleBillboardScripts[i].SetAlphaDistance(alphaDecrementStartDistance);

                testParticleBillboardScripts[i].UpdateParticle();
            }


        }

        GameObject createUnderFog()
        {
            return underFogParticle.instantiate(vec3.Zero, stageFolder);
        }
        GameObject createBillboardFog()
        {
            return billboardFogParticle.instantiate(vec3.Zero, stageFolder);
        }

        vec3 getTestParticleInitPos(vec3 centerPos, float ratioX, float ratioZ, float randomY)
        {
            vec3 pos;
            pos.x = random.genF32() % 1 * 2 * (AreaScaleX) + centerPos.x - AreaScaleX;
            pos.y = centerPos.y + random.genF32() * randomY;
            pos.z = random.genF32() % 1 * 2 * (AreaScaleZ) + centerPos.z - AreaScaleZ;

            return pos;
        }
        vec3 getParticleInitPos(GameObject fogArea, float randomY)
        {
            vec3 centerPos = fogArea.Transform.Position;

            vec3 pos;
            pos.x = random.genF32() % 1 * (2 * fogArea.Transform.LocalScale.x) + centerPos.x - fogArea.Transform.LocalScale.x;
            pos.y = centerPos.y + random.genF32() * randomY;
            pos.z = random.genF32() % 1 * (2 * fogArea.Transform.LocalScale.z) + centerPos.z - fogArea.Transform.LocalScale.z;

            return pos;
        }


        //オブジェクトプール内から補給しながら生成、なければここで新規生成
        public void CreateParticles(GameObject fogArea)
        {
            var fogAreaScript = fogArea.getComponent<FogArea>();

            areaContainer.SetArea(fogAreaScript);

            vec3 centerPos = fogArea.Transform.Position;
            //基準の大きさとの比
            float ratioX = fogArea.Transform.LocalScale.x / AreaScaleX;
            float ratioZ = fogArea.Transform.LocalScale.z / AreaScaleZ;

            //必要な数
            int needUnderFogNum = (int)(particleUnderNum * ratioX * ratioZ * 2);
            int needBillboardFogNum = (int)(particleBillboardNum * ratioX * ratioZ * 2);

            //補給可能な数
            int freeUnderFogNum = particleUnderScripts.Count;
            int freeBillboardFogNum = particleBillboardScripts.Count;

            //補給可能数が必要な数より多ければ必要量に置き換える
            if (freeUnderFogNum >= needUnderFogNum) freeUnderFogNum = needUnderFogNum;
            if (freeBillboardFogNum >= needBillboardFogNum) freeBillboardFogNum = needBillboardFogNum;

            if (freeUnderFogNum > 0)
            {
                for (int i = 0; i < freeUnderFogNum; i++)
                {
                    fogAreaScript.SetUnderFogScript(particleUnderScripts.Pop(), getParticleInitPos(fogArea, underFogRandomFactor));
                }
            }

            if (freeBillboardFogNum > 0)
            {
                for (int i = 0; i < freeBillboardFogNum; i++)
                {
                    fogAreaScript.SetBillboardFogScript(particleBillboardScripts.Pop(), getParticleInitPos(fogArea, billboardFogRandomFactor));
                }
            }

            //足りない数
            int rackUnderFogNum = needUnderFogNum - freeUnderFogNum;
            int rackBillboardFogNum = needBillboardFogNum - freeBillboardFogNum;

            //足りない数が０より大きければ生成
            if (rackUnderFogNum > 0)
            {
                for (int i = 0; i < rackUnderFogNum; i++)
                {

                    if (underFogParticle == null)
                    {
                        via.debug.errorLine("FogParticleManager:underFogParticleが設定されていません");
                        return;
                    }

                    vec3 pos = getParticleInitPos(fogArea, underFogRandomFactor);
                    GameObject particle = underFogParticle.instantiate(pos, stageFolder);
                    FogParticle_Under script = particle.getComponent<FogParticle_Under>();

                    fogAreaScript.SetUnderFogScript(script, pos);
                    script.SetInitPos(pos);
                }
            }
            if (rackBillboardFogNum > 0)
            {
                for (int i = 0; i < rackBillboardFogNum; i++)
                {

                    if (billboardFogParticle == null)
                    {
                        via.debug.errorLine("FogParticleManager:fogParticleBillboardが設定されていません");
                        return;
                    }

                    vec3 pos = getParticleInitPos(fogArea, billboardFogRandomFactor);
                    GameObject particleBillBoard = billboardFogParticle.instantiate(pos, stageFolder);
                    FogParticle_Billboard scriptBillboard = particleBillBoard.getComponent<FogParticle_Billboard>();

                    fogAreaScript.SetBillboardFogScript(scriptBillboard, pos);
                    scriptBillboard.SetInitPos(pos);
                }
            }

            debug.infoLine($"""
                生成直後在庫オブジェクト数 
                Under'{particleUnderScripts.Count}'個
                Billboard'{particleBillboardScripts.Count}'個 
                合計'{particleUnderScripts.Count + particleBillboardScripts.Count}'個

                生成したオブジェクト数
                Under'{rackUnderFogNum}'個
                Billboard'{rackBillboardFogNum}'個 
                合計'{rackUnderFogNum + rackBillboardFogNum}'個
                """);
        }
        public void ReleaseParticles(GameObject fogArea)
        {
            var fogAreaScript = fogArea.getSameComponent<FogArea>();
            if (fogAreaScript == null)
            {
                debug.errorLine("fogAreaScriptが取得できません");
            }

            areaContainer.ReleaseArea(fogAreaScript);

            var underFogLinkedList = fogAreaScript.ReleaseUnderFogScript();

            var uNode = underFogLinkedList.First;
            while (uNode != null)
            {
                var next = uNode.Next;

                particleUnderScripts.Push(uNode.Value);
                //debug.infoLine("ReleasedUnderFog");
                uNode.Value.SetActiveMesh(false);
                underFogLinkedList.Remove(uNode);

                uNode = next;
            }

            var billboardFogLinkedList = fogAreaScript.ReleaseBillboardFogScript();
            var bNode = billboardFogLinkedList.First;
            while (bNode != null)
            {
                var next = bNode.Next;

                particleBillboardScripts.Push(bNode.Value);
                //debug.infoLine("ReleasedBillboardFog");
                bNode.Value.SetActiveMesh(false);
                billboardFogLinkedList.Remove(bNode);

                bNode = next;
            }

            debug.infoLine($"""
                リリース直後在庫オブジェクト数 
                Under'{particleUnderScripts.Count}'個
                Billboard'{particleBillboardScripts.Count}'個 
                合計'{particleUnderScripts.Count + particleBillboardScripts.Count}'個
                """);
        }

    }
}
