using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;
using static Unity.VisualScripting.Member;
using System;

public class ConfigManager : MonoBehaviour
{
    [System.Serializable]
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    public struct configParameter
    {
        public GameObject configPanel;
        public EaseFunc func;
        public SettingUI nextApplyPanel;
        public Text text;
       
    }
    [SerializeField] private List<configParameter> configObjs; 
    [SerializeField] private List<Transform> refPoints;
    [SerializeField] private float easeTime = 0.2f;
    [SerializeField] private iTween.EaseType easeType;

    SettingUI ui;
    bool isTiltedOne = false;
    bool isApply = false;
    bool isEase = false;
    float timer = 0.0f;
    [SerializeField] int index = 0;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        ui = GetComponent<SettingUI>();
        for (int i = 0; i < configObjs.Count; i++)
        {
            configObjs[i].configPanel.transform.position = refPoints[i].transform.position;
            configObjs[i].text.enabled = false;
        }
       
    }

        // Update is called once per frame
        void Update()
        {
            if (ui != null && ui.GetPanelState() == SettingUI.ePanelState.PanelState_Select)
            {
                if (isEase)//補間時間の間は操作をロックする
                {
                    timer += Time.deltaTime;
                    if (timer > easeTime)
                    {
                    for(int i=0;i< configObjs.Count;i++)
                    {
                        configObjs[i].text.enabled = false;
                      }
                    configObjs[index].text.enabled = true;
                    timer = 0.0f;
                        isEase = false;
                    }
                }
                if (isTiltedOne == false && isApply == false)
                {

                    bool valueApply = InputUtil.Instance.IsAnyEastPressed();

                    if (valueApply)
                    {
                        isApply = true;

                    }
                    else if (InputUtil.Instance.IsAnyAxisChangedToRight())//右
                    {


                        if (isEase == false)
                        {
                            Debug.Log("Ease");

                        index++;
                        
                        for (int i = 0; i < configObjs.Count; i++)
                            {
                            iTween.MoveTo(configObjs[i].configPanel, iTween.Hash(
                                "position", refPoints[(i + index) % refPoints.Count].transform.localPosition,
                                "time", easeTime,
                                "easetype", easeType.ToString(),
                                "isLocal", true)
                                );
                            }
                        if (index >= configObjs.Count) index = 0;
                        isEase = true;
                        }
                        isTiltedOne = true;
                    ui.SetNextSelectedPanelApply(configObjs[index].nextApplyPanel);
                    ui.SetEaseFuncApply(configObjs[index].func);

                }
                    else if (InputUtil.Instance.IsAnyAxisChangedToLeft())//左
                    {
                        if (isEase == false)
                        {
                            Debug.Log("Ease");

                        index--;
                        if (index < 0) index = configObjs.Count - 1;
                        for (int i = 0; i < configObjs.Count; i++)
                            {
                                iTween.MoveTo(configObjs[i].configPanel, iTween.Hash(
                                "position", refPoints[(i + index + refPoints.Count) % (refPoints.Count)].transform.localPosition,
                                "time", easeTime,
                                "easetype", easeType.ToString(),
                                "isLocal", true)
                                );
                            }
                        
                        isEase = true;
                        }
                        isTiltedOne = true;
                    ui.SetNextSelectedPanelApply(configObjs[index].nextApplyPanel);
                    ui.SetEaseFuncApply(configObjs[index].func);

                }
                   
                }
                if (InputUtil.Instance.IsAnyAxisChanged() == false)
                {
                    isTiltedOne = false;
                }

            if(isApply)
            {
                ui.SetIsChooseEnabled(true);
             
                ui.SetPanelState(SettingUI.ePanelState.PanelState_Choose,true);
            }
            else
            {
                ui.SetIsChooseEnabled(false);
            }
            


            }
        }
    
}
