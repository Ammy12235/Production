//=============================================================================
// <summary>
// FogAreaContainer 
// </summary>
// <author>CGC_12_堀 大輔</author>
//=============================================================================
using System.Collections.Generic;
using via;
using via.attribute;
using static blackfilter.FogParticleManager;

namespace blackfilter
{   /// <summary>
    ///フォグエリアコンテナクラス：フォグエリアの更新処理を一括で行うクラス。
    /// </summary>
    public class FogAreaContainer : via.Behavior
    {

        //[DataMember] private List<GameObject> areaContainer;

        [DataMember] private List<FogArea> areaContainer = new();
        public void InitContainer()
        {
            //コンテナの初期化
        }

        public void SetArea(FogArea area)
        {
            if (areaContainer.Contains(area))
            {
                return;
            }
            else
            {
                debug.infoLine("新しいエリアがコンテナに追加されました");
                areaContainer.Add(area);

            }
        }

        public void ReleaseArea(FogArea area)
        {
            if (areaContainer.Contains(area))
            {
                debug.infoLine("エリアがコンテナから削除されました");
                areaContainer.Remove(area);
            }
            else return;
        }

        public void UpdateContainer(particleParameter parameter)
        {

            int count = areaContainer.Count;
            if (count == 0) return;
            for (int i = 0; i < count; i++)
            {
                areaContainer[i].UpdateArea(parameter);
            }
        }

        public int GetAreaNum()
        {
            return areaContainer.Count;
        }
    }
}
