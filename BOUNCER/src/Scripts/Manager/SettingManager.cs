using NUnit.Framework;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using Unity.Collections;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Audio;
using UnityEngine.UI;

public enum eSettingState
{
    SettingState_None = -1,
    SettingState_Menu,
    SettingState_Rule,
    SettingState_MemberSetting,
    SettingState_HelpInfo,
    SettingState_Config,
    SettingState_End,
    SettingState_StuffRoll,
    SettingState_Quit,
}

public class SettingManager : MonoBehaviour
{
    public static SettingManager instance;

    [SerializeField] private SettingUI initSelectUI;
    [SerializeField] private FollowingCamera camera;
    [SerializeField] private EaseFunc initFunc;
    [SerializeField] private EaseFunc quitFunc;
    private Stack<EaseFunc> funcStack = new();
    private EaseFunc stackFunc;
    [SerializeField] private SettingUI currentUI;
    private System.Action lateFunc;

    private SettingUI previousUI;
    [SerializeField] private List<GameObject> guideUIRight = new();
    [SerializeField] private List<GameObject> guideUILeft = new();
    [SerializeField] private List<RawImage> guideUIRightImage = new();
    [SerializeField] private List<RawImage> guideUILeftImage = new();
    [SerializeField] private float guideUIOffsetY;
    [SerializeField] private float guideAppearTime = 2;

    [SerializeField] private RawImage menuBackGround;

    [SerializeField] private AudioSource audioSource;
    [SerializeField] private AudioClip select;
    [SerializeField] private AudioClip undo;
    [SerializeField] private AudioClip apply;

    [Header("全員パッド登録完了時のSE")]
    [SerializeField] private AudioClip allPadAssignedSE;

    private eSettingState _state = eSettingState.SettingState_Menu;

    [SerializeField, ReadOnly] private e2DUIPattern currentGuideUIPattern;
    float guideAlpha = 0.0f;
    bool isTiltedOne = false;
    bool isChoose = false;
    bool isUndo = false;
    bool isEase = false;
    bool isInitEnd = false;
    bool isGuide = true;
    float initFuncTime = 0;
    float timer = 0.0f;
    float guideTimer = 0.0f;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        for (int i = 0; i < guideUIRight.Count; i++) guideUIRightImage.Add(guideUIRight[i].GetComponent<RawImage>());
        for (int i = 0; i < guideUILeft.Count; i++) guideUILeftImage.Add(guideUILeft[i].GetComponent<RawImage>());
        isInitEnd = false;
        initFuncTime = initFunc.GetLongestTime();
        lateFunc = initFunc.LateProceed;

        //やめるボタンをスタックに入れておく
        funcStack.Push(quitFunc);
        initFunc.SetStartPos(false);
    }

    private void Awake()
    {

        if (instance == null)
        {
            instance = this;
            //DontDestroyOnLoad(gameObject); // �V�[���ԂŃ}�l�[�W���[��ێ�����ꍇ
        }
        else
        {
            Destroy(gameObject);
        }
    }

    public eSettingState GetSettingState()
    {
        return _state;
    }

    public void SetSettingState(eSettingState state)
    {
        _state = state;
    }

    public AudioClip GetAllPadAssignedSE()
    {
        return allPadAssignedSE;
    }

    public AudioSource GetAudioSource()
    {
        return audioSource;
    }

    private void UpdateUIPattern(e2DUIPattern pattern)
    {
        if (pattern == e2DUIPattern.Pattern_None)
        {
            guideUIRightImage[0].color = new Color(1, 1, 1, iTween.FloatUpdate(guideUIRightImage[0].color.a, 0, 20));
            guideUILeftImage[0].color = new Color(1, 1, 1, iTween.FloatUpdate(guideUILeftImage[0].color.a, 0, 20));
        }
        else if (pattern == e2DUIPattern.Pattern_Default)
        {
            guideUIRightImage[0].color = new Color(1, 1, 1, iTween.FloatUpdate(guideUIRightImage[0].color.a, 1, 20));
            guideUILeftImage[0].color = new Color(1, 1, 1, iTween.FloatUpdate(guideUILeftImage[0].color.a, 1, 20));
        }
        else if (pattern == e2DUIPattern.Pattern_Config)
        {

        }
        else if (pattern == e2DUIPattern.Pattern_OnlyUndo)
        {
            guideUILeftImage[0].color = new Color(1, 1, 1, iTween.FloatUpdate(guideUILeftImage[0].color.a, 1, 20));
        }
    }

    // Update is called once per frame
    void Update()
    {
        //一番最初の遷移が終了した
        if (isInitEnd == false)
        {
            timer += Time.deltaTime;
            initFunc.Proceed();
            if (initFuncTime < timer)
            {
                Debug.Log("InitCompleted");
                currentUI = initSelectUI;
                previousUI = currentUI;
                currentUI.SetPanelState(SettingUI.ePanelState.PanelState_Select);

                isInitEnd = true;
                timer = 0.0f;

            }
            return;
        }
        //選択状態で、選択アニメーションが終了したら
        if (isChoose)
        {

            if (currentUI.GetAnimator() != null)//アニメーターが登録されていたら
            {
                if (currentUI.GetAnimator().GetCurrentAnimatorStateInfo(0).IsTag("choose") &&
                    currentUI.GetAnimator().GetCurrentAnimatorStateInfo(0).normalizedTime >= 1 && isEase == false)
                {
                    lateFunc = currentUI.GetEaseFuncApply().LateProceed;//LateUpdate()内で呼び出されるアクション
                    currentGuideUIPattern = currentUI.GetEaseFuncApply().GetNextGuideUIPattern();

                    isEase = true;
                }
            }
            else
            {
                lateFunc = currentUI.GetEaseFuncApply().LateProceed;
                currentGuideUIPattern = currentUI.GetEaseFuncApply().GetNextGuideUIPattern();
                isEase = true;
            }

            if (isEase)
            {

                timer += Time.deltaTime;

                currentUI.GetEaseFuncApply().Proceed();//正方向に実行

                guideUIRightImage[0].color = new Color(1, 1, 1, iTween.FloatUpdate(guideUIRightImage[0].color.a, 0, 20));
                guideUILeftImage[0].color = new Color(1, 1, 1, iTween.FloatUpdate(guideUIRightImage[0].color.a, 0, 20));

                if (currentUI.GetEaseFuncApply().GetLongestTime() < timer)//登録されている遷移オブジェクトのうち一番遷移に時間のかかるオブジェクトが終点に達した
                {
                    Debug.Log("ApplyEaseCompleted");
                    SettingUI next = currentUI.GetNextSelectedPanelApply();
                    if (next != null) next.SetPanelState(SettingUI.ePanelState.PanelState_Select);
                    currentUI.GetEaseFuncApply().End();//終了処理
                    lateFunc = null;

                    currentUI.SetPanelState(SettingUI.ePanelState.PanelState_NoSelect);

                    previousUI = next;
                    currentUI = next;


                    timer = 0.0f;
                    isChoose = false;
                    isEase = false;
                }

                return;
            }
        }
        if (isUndo)//スタックからイージング関数をポップして実行
        {
            timer += Time.deltaTime;
            stackFunc.Reverse();//逆方向に実行

            guideUIRightImage[0].color = new Color(1, 1, 1, iTween.FloatUpdate(guideUIRightImage[0].color.a, 0, 20));
            guideUILeftImage[0].color = new Color(1, 1, 1, iTween.FloatUpdate(guideUIRightImage[0].color.a, 0, 20));
            if (stackFunc.GetLongestTime() < timer)
            {
                Debug.Log("UndoEaseCompleted");
                SettingUI next = currentUI.GetNextSelectedPanelUndo();
                if (next != null) next.SetPanelState(SettingUI.ePanelState.PanelState_Select);

                stackFunc.Entry();

                currentGuideUIPattern = stackFunc.GetCurrentGuideUIPattern();
                lateFunc = null;
                currentUI.SetPanelState(SettingUI.ePanelState.PanelState_NoSelect);
                previousUI = next;
                currentUI = next;
                if (currentUI.GetIsSelectSoundPlayAtUndoEnd())
                    audioSource.PlayOneShot(select);


                timer = 0.0f;
                isUndo = false;
                isEase = false;
            }
            return;
        }
        if (isChoose == false && isUndo == false)
        {
            guideTimer += Time.deltaTime;
            if (guideTimer > guideAppearTime)
            {
                UpdateUIPattern(currentGuideUIPattern);
            }
            else
            {
                UpdateUIPattern(e2DUIPattern.Pattern_None);
            }

        }


        //パッドとキーボードから設定画面を操作する
        if (isTiltedOne == false && isChoose == false)//入力をニュートラルに戻した、かつ選択状態でないとき
        {
            bool valueApply = InputUtil.Instance.IsAnyEastPressed();
            bool valueUndo = InputUtil.Instance.IsAnySouthPressed();
            bool isCooseEnabled = currentUI.GetIsChooseEnabled();
            if (currentUI.GetEaseFuncApply() != null && ((valueApply && isCooseEnabled) || (currentUI.GetIsForceEase() && !isCooseEnabled)))//イージング関数が登録されていて、適切な条件がそろっているならば
            {

                currentUI.SetPanelState(SettingUI.ePanelState.PanelState_Choose);//選択状態にする
                currentUI.SetIsForceEase(false);
                currentUI.GetEaseFuncApply().SetStartPos(false);//正方向で実行するための初期化
                currentUI.GetEaseFuncApply().Entry();//初期化処理

                funcStack.Push(currentUI.GetEaseFuncApply());
                audioSource.PlayOneShot(apply);
                isChoose = true;

                return;
            }
            else if (valueUndo && currentUI.GetIsUndoEnabled() == true)
            {

                if (currentUI.GetIsReverseByStackFunc())//もしスタックにある補間関数を使うのなら
                {
                    if (funcStack != null && funcStack.Count != 0)
                    {
                        stackFunc = funcStack.Pop();
                    }
                    else if (funcStack.Count == 0)//もしスタックが空ならばQuitFuncを実行する
                    {
                        stackFunc = quitFunc;
                        currentGuideUIPattern = currentUI.GetEaseFuncApply().GetNextGuideUIPattern();//基本的にNone
                    }
                }
                else//用意した関数が使いたければ
                {

                    stackFunc = currentUI.GetEaseFuncUndo();
                    int popCount = currentUI.GetPopCount();

                    for (int i = 0; i < popCount; i++)
                    {

                        if (funcStack != null && funcStack.Count != 0)
                        {

                            funcStack.Pop();
                        }
                    }


                }
                stackFunc.End();

                lateFunc = stackFunc.LateReverse;
                audioSource.PlayOneShot(undo);
                isUndo = true;
                return;

            }
            else if (InputUtil.Instance.IsAnyAxisChangedToRight())//インデックスで上下左右を指定
            {
                currentUI = previousUI.GetNeighborPanel(3);
                isTiltedOne = true;
            }
            else if (InputUtil.Instance.IsAnyAxisChangedToLeft())
            {
                currentUI = previousUI.GetNeighborPanel(2);
                isTiltedOne = true;
            }
            else if (InputUtil.Instance.IsAnyAxisChangedToUp())
            {
                currentUI = previousUI.GetNeighborPanel(0);
                isTiltedOne = true;
            }
            else if (InputUtil.Instance.IsAnyAxisChangedToDown())
            {
                currentUI = previousUI.GetNeighborPanel(1);
                isTiltedOne = true;
            }

            if (currentUI == null)
            {
                currentUI = previousUI;
            }
            else if (InputUtil.Instance.IsAnyAxisChanged() && currentUI != null)
            {

                previousUI.SetPanelState(SettingUI.ePanelState.PanelState_NoSelect);
                audioSource.PlayOneShot(select);
                previousUI = currentUI;
                currentUI.SetPanelState(SettingUI.ePanelState.PanelState_Select);
                guideTimer = 0.0f;
            }
        }


        if (InputUtil.Instance.IsAnyAxisChanged() == false)
        {
            isTiltedOne = false;
        }

    }

    private void LateUpdate()
    {

        lateFunc?.Invoke();

    }
}
