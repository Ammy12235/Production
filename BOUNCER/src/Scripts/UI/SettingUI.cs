using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

public class SettingUI : MonoBehaviour
{
    [SerializeField, Header("上下左右のパネルを指定")] private SettingUI upPanel;
    [SerializeField] private SettingUI downPanel;
    [SerializeField] private SettingUI leftPanel;
    [SerializeField] private SettingUI rightPanel;

    [SerializeField, Header("決定ボタン押下した時の補完")] private EaseFunc applyFunc;
    [SerializeField] private SettingUI nextSelectedApplyPanel;

    [SerializeField, Header("リバースフラグをオンにするまたはundoFuncを割り当てる\n戻るボタン押下した時の補完")] private bool isReverseByStackFunc = false;
    [SerializeField] private EaseFunc undoFunc;
    [SerializeField] private int popCount = 1;
    [SerializeField] private SettingUI nextSelectedUndoPanel;

    [SerializeField, Header("パネルそのものについているアニメーター")] private Animator anim;
    [SerializeField] private GameObject effectObj;
    [SerializeField, Header("選択エフェクトについているアニメーター")] private Animator effectAnim;
    [SerializeField] private Material material;
    [SerializeField, Header("Select状態時の拡大率")] private float selectExtendScale = 1.3f;
    [SerializeField] private Text explainText;
    [SerializeField] private Transform offsetTextStartPos;
    [SerializeField] private Transform offsetTextEndPos;

    [SerializeField] private bool isSelectSoundPlayAtUndoEnd = true;
    [SerializeField] private bool isForceEase = false;
    [SerializeField] private bool isChooseEnabled = true;
    [SerializeField] private bool isUndoEnabled = true;
    Vector3 initScl;
    Color initCol;
    bool isChoose = false;

    public enum ePanelState
    {
        PanelState_None = -1,
        PanelState_Interpolate,
        PanelState_NoSelect,
        PanelState_Select,
        PanelState_Choose,
    }

    private ePanelState _state;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        if (material != null)
            initCol = material.GetColor("_EmissionColor");
        gameObject.SetActive(true);
        initScl = transform.localScale;

        if (explainText != null)
        {

            explainText.rectTransform.position = offsetTextStartPos.position;
        }

        if (effectAnim != null) effectAnim.enabled = false;
    }

    public void SetPanelState(ePanelState state,bool isChooseFlagOn =false)
    {
        _state = state;
        
        if (_state == ePanelState.PanelState_Select)
        {
            if (effectAnim != null && effectAnim.isActiveAndEnabled)
                effectAnim.Play(effectAnim.GetCurrentAnimatorStateInfo(0).shortNameHash, 0, 0.5f);
        }

        if (isChooseFlagOn)
        {
            isChoose = true;
        }
        else return;
    }

    public void SetIsChooseEnabled(bool canChoose)
    {
        isChooseEnabled = canChoose;
    }

    public void SetIsUndoEnabled(bool canUndo)
    {
        isUndoEnabled = canUndo;
    }

    public void SetEaseFuncApply(EaseFunc func)
    {
        applyFunc = func;
    }

    public void SetNextSelectedPanelApply(SettingUI nextPanel)
    {
        nextSelectedApplyPanel = nextPanel;
    }

    public bool GetIsForceEase()
    {
        return isForceEase;
    }
    public void SetIsForceEase(bool isForce)
    {
        isForceEase = isForce;
    }

    public bool GetIsChooseEnabled()
    {
        return isChooseEnabled;
    }

    public bool GetIsUndoEnabled()
    {
        return isUndoEnabled;
    }

    public int GetPopCount()
    {
        return popCount;
    }

    public void SetPopCount(int num)
    {
        popCount = num;
    }

    public ePanelState GetPanelState()
    {
        return _state;
    }

    public bool GetIsReverseByStackFunc()
    {
        return isReverseByStackFunc;
    }

    public bool GetIsSelectSoundPlayAtUndoEnd()
    {
        return isSelectSoundPlayAtUndoEnd;
    }

    public Animator GetAnimator()
    {
        if (anim != null)
            return anim;
        else return null;
    }

    public SettingUI GetNextSelectedPanelApply()
    {
        if (nextSelectedApplyPanel != null)
            return nextSelectedApplyPanel;
        else return null;
    }

    public SettingUI GetNextSelectedPanelUndo()
    {
        if (nextSelectedUndoPanel != null)
            return nextSelectedUndoPanel;
        else return null;
    }

    public SettingUI GetNeighborPanel(int index)
    {
        if (index == 0) return upPanel;
        else if (index == 1) return downPanel;
        else if (index == 2) return leftPanel;
        else if (index == 3) return rightPanel;
        else return null;
    }

    public EaseFunc GetEaseFuncApply()
    {
        if (applyFunc != null)
            return applyFunc;
        else return null;
    }

    public EaseFunc GetEaseFuncUndo()
    {
        if (undoFunc != null)
            return undoFunc;
        else return null;
    }




    // Update is called once per frame
    public void Update()
    {

        /*
        if (_state == ePanelState.PanelState_Interpolate)
        {
            SetInterpolate();
        }
         */
        if (_state == ePanelState.PanelState_Select)//�Z���N�g��ԂȂ�
        {
            if (anim != null)
                anim.SetInteger("state", (int)_state);
            if (effectAnim != null)
            {
                effectObj.SetActive(true);
                effectAnim.enabled = true;
            }
            if (explainText != null)
            {
                if (isChoose)
                {
                    explainText.color = new Color(1, 1, 1,1);
                    explainText.rectTransform.position = offsetTextStartPos.position;
                    isChoose = false;
                }
                explainText.rectTransform.position = iTween.Vector3Update(explainText.rectTransform.position, offsetTextEndPos.position, 20);
                explainText.enabled = true;
            }

            iTween.ScaleUpdate(this.gameObject, iTween.Hash(
             "scale", initScl * selectExtendScale,
             "time", 0.2f,
             "easeType", "easeOutQuint")
         );
        }
        else if (_state == ePanelState.PanelState_NoSelect)
        {
            if (anim != null)
                anim.SetInteger("state", (int)_state);
            if (effectAnim != null)
            {
                //effectAnim.Play(effectAnim.GetCurrentAnimatorStateInfo(0).shortNameHash, 0,1);
                effectObj.SetActive(false);
                effectAnim.enabled = false;
            }
            if (explainText != null)
            {
                explainText.rectTransform.position = offsetTextStartPos.position;
                explainText.enabled = false;

            }

            iTween.ScaleUpdate(this.gameObject, iTween.Hash(
            "scale", initScl,
            "time", 0.2f,
            "easeType", "easeOutBack")
        );
        }
        else if (_state == ePanelState.PanelState_Choose)
        {
            if (anim != null)
            {
                anim.SetInteger("state", (int)_state);
            }
            if (explainText != null)
            {
               
                explainText.rectTransform.position = iTween.Vector3Update(explainText.rectTransform.position, offsetTextStartPos.position, 20);
                explainText.color -= new Color(0, 0, 0, Time.deltaTime * 2);
                explainText.enabled = true;
            }
            isChoose = true;
        }
    }
}
