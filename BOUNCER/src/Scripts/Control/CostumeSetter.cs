using NUnit.Framework;
using UnityEngine;
using System.Collections.Generic;

public class CostumeSetter : MonoBehaviour
{
    [SerializeField] private List<GameObject> childCostume;
    int costumeIndex = 0;
    public void SetCostume(int index)
    {
        if (index >= 0 && index <= childCostume.Count)
        {

            for (int i = 0; i < childCostume.Count; i++)
            {
                childCostume[i].SetActive(false);
            }

            if (index != 0)
            {
                childCostume[index - 1].SetActive(true);
            }
            costumeIndex = index;
        }

    }

    public int GetCurrentCostumeIndex()
    {
        return costumeIndex;
    }

}
