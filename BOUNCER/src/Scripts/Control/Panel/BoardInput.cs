using NUnit.Framework;
using UnityEngine;
using UnityEngine.Video;
using System.Collections.Generic;

public class BoardInput : MonoBehaviour
{
    [SerializeField] BoardOutput output;
    [SerializeField] List<VideoClip> clips;
    bool isSelect = false;
    SettingUI ui;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        ui = GetComponent<SettingUI>();
    }

    public List<VideoClip> GetVideoClips()
    {
        return clips;
    }

    // Update is called once per frame
    void Update()
    {
        if (ui != null)
        {
            if (ui.GetPanelState() == SettingUI.ePanelState.PanelState_Select&&isSelect==false)//選択されたら伝達し、ロックする
            {
                output.NotifyInput(this);
                isSelect = true;
            }
            else if (ui.GetPanelState() == SettingUI.ePanelState.PanelState_NoSelect|| ui.GetPanelState() == SettingUI.ePanelState.PanelState_NoSelect)//未選択状態または決定状態ならロックを解除する
            {
                isSelect = false;
            }
        }
    }
}
