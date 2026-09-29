using NUnit.Framework;
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Video;
using static Unity.VisualScripting.Member;

public class RuleExpManager : MonoBehaviour
{
    [SerializeField] private List<GameObject> categories;
    [SerializeField] private VideoPlayer videoPlayer;

    [System.Serializable]
    public struct ruleExpParameters
    {
        public List<ruleExpParameter> param;

    }

    [System.Serializable]
    public struct ruleExpParameter
    {
        public GameObject panelOn;
        public GameObject panelOff;
        public VideoClip clip;
        public GameObject explainText;

    }


    [SerializeField] private List<ruleExpParameters> expParameters;
    int rowIndex = 0;//横
    int columnIndex = 0;//縦

    bool isTiltedOne = false;
    SettingUI ui;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        SetPanelIndex(rowIndex);
        ui = GetComponent<SettingUI>();
        videoPlayer.loopPointReached += LoopPointReached;
    }

    private void LoopPointReached(VideoPlayer player)
    {
        videoPlayer.Pause();
        player.clip = expParameters[rowIndex].param[columnIndex].clip;//次に登録されたクリップを再生
        videoPlayer.Play();
    }

    // Update is called once per frame
    void Update()
    {
        if (ui != null && ui.GetPanelState() == SettingUI.ePanelState.PanelState_Select)
        {

            if (isTiltedOne == false)
            {
                if (InputUtil.Instance.IsAnyAxisChangedToRight())//右
                {
                    if (rowIndex < categories.Count - 1)
                    {

                        rowIndex++;
                        SetPanelIndex(rowIndex);
                    }


                    isTiltedOne = true;
                }
                else if (InputUtil.Instance.IsAnyAxisChangedToLeft())//左
                {
                    if (rowIndex > 0)
                    {
                        rowIndex--;
                        SetPanelIndex(rowIndex);
                    }

                    isTiltedOne = true;
                }
                else if (InputUtil.Instance.IsAnyAxisChangedToUp())//上（上下の移動でcolumnIndexを切り替える）
                {
                    if (columnIndex > 0)
                    {
                        int previousIndex = columnIndex;
                        columnIndex--;
                        int currentIndex = columnIndex;
                        SetPanelColumnIndex(currentIndex, previousIndex);
                    }
                    isTiltedOne = true;
                }
                else if (InputUtil.Instance.IsAnyAxisChangedToDown())//下
                {
                    if (columnIndex < expParameters[rowIndex].param.Count - 1)
                    {
                        int previousIndex = columnIndex;
                        columnIndex++;
                        int currentIndex = columnIndex;
                        SetPanelColumnIndex(currentIndex, previousIndex);
                    }
                    isTiltedOne = true;
                }
            }
            if (InputUtil.Instance.IsAnyAxisChanged() == false)
            {
                isTiltedOne = false;
            }

        }
    }

    private void SetPanelIndex(int index)
    {
        //カテゴリーごとの有効化
        for (int i = 0; i < categories.Count; i++)
        {
            categories[i].SetActive(false);

        }
        categories[index].SetActive(true);

        /*
        //カテゴリー内の構成物の有効化
        for (int i = 0; i < expParameters.Count; i++)
        {
            for (int j = 0; j < expParameters[i].param.Count; j++)
            {
                expParameters[i].param[j].panelOff?.SetActive(false);
                expParameters[i].param[j].panelOn?.SetActive(false);
            }

        }
          */

        //一旦すべての項目を無効にする
        if (expParameters[index].param[0].panelOff == null) return;
        for (int j = 0; j < expParameters[index].param.Count; j++)
        {
            expParameters[index].param[j].panelOff?.SetActive(true);
            expParameters[index].param[j].panelOn?.SetActive(false);
            expParameters[rowIndex].param[j].explainText.SetActive(false);

        }
        //一番上の項目を有効にする
        expParameters[index].param[0].panelOn?.SetActive(true);
        expParameters[index].param[0].panelOff?.SetActive(false);
        expParameters[index].param[0].explainText.SetActive(true);
        if(expParameters[index].param[0].clip!=null)
        videoPlayer.clip= expParameters[index].param[0].clip;
        columnIndex = 0;
    }

    private void SetPanelColumnIndex(int currentIndex, int prevIndex)
    {
        //列内の有効化、フリップ
        expParameters[rowIndex].param[currentIndex].panelOff?.SetActive(false);
        expParameters[rowIndex].param[currentIndex].panelOn?.SetActive(true);

        expParameters[rowIndex].param[prevIndex].panelOff?.SetActive(true);
        expParameters[rowIndex].param[prevIndex].panelOn?.SetActive(false);

        VideoClip clip = expParameters[rowIndex].param[currentIndex].clip;
        if (clip != null)
        {
            videoPlayer.clip = clip;

        }
        else
        {


        }

        GameObject textObj = expParameters[rowIndex].param[currentIndex].explainText;
        if (textObj != null)
        {
            expParameters[rowIndex].param[currentIndex].explainText.SetActive(true);
            expParameters[rowIndex].param[prevIndex].explainText.SetActive(false);
        }
    }
}
