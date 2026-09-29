using UnityEngine;
using System.Collections.Generic;
using Unity.Collections;

public class CameraSystem : Singleton<CameraSystem>
{

    [SerializeField] private GameObject _mainTarget;
    [SerializeField] private bool _isCameraShake = false;
    [SerializeField, Header("カメラの目標地点に到達する際のディレイ（あそび）")]
    private float _positionDelay;
    [SerializeField]
    private float _rotationDelay;

    private ICameraState _state;
    private ICameraState _prevState;


    private Vector3 eventStartPos;
    private Quaternion eventStartRot;

    private Vector3 eventDestPos;
    private Quaternion eventDestRot;

    private Vector3 eventFinalDestPos;
    private Quaternion eventFinalDestRot;


    private Dictionary<uint, CameraTrigger> _cameraTriggers = new();

    static uint m_pCameraStateID = 0;

    private Event_Camera _eventCamera;
    private float easeTime = 0.0f;
    private float easeTimer = 0.0f;

    private Vector3 _velocity;

    enum CameraState
    {
        CameraState_Default,//通常動作
        CameraState_Interpolate,//ステート間の補間中
        CameraState_Event,//イベントカメラ
        CameraState_Stop//カメラ停止
    }

    CameraState cameraSystemState = CameraState.CameraState_Default;
    CameraState prevCameraSystemState = CameraState.CameraState_Default;

    protected override bool UseDontDestroyOnLoad => false;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {

    }

#if UNITY_EDITOR
    void OnDrawGizmos()
    {
        Gizmos.color = new Color(0, 1, 0.5f, 1);//水色

        Vector3 pos=Vector3.zero;
        if (_state != null)
        {
            pos = _state.GetDestPosition();
        }
        Gizmos.DrawWireSphere(pos, 1);

        Gizmos.DrawLine(transform.position, pos);
    }
#endif

    // Update is called once per frame
    void LateUpdate()
    {
        SolveCameraState();//次のステートを決定

        if (_state != null)
        {

            float positionDelay = 0;
            float rotationDelay = 0;
            _state.Run();//決定されたステートに従ってカメラの最終 transformを算出

            //ステートがデフォルトならば
            if (cameraSystemState == CameraState.CameraState_Default)
            {
                positionDelay = _positionDelay;
                rotationDelay = _rotationDelay;

                transform.position = Vector3.SmoothDamp(transform.position, _state.GetDestPosition(), ref _velocity, positionDelay);
                transform.rotation = Quaternion.Slerp(transform.rotation, _state.GetDestRotation(), rotationDelay * Time.deltaTime);
            }
            //補間モードならば
            else if (cameraSystemState == CameraState.CameraState_Interpolate)
            {
                positionDelay = _positionDelay * easeTime;
                rotationDelay = _rotationDelay;

                easeTimer += Time.deltaTime;
                //遷移時間を超えたら、
                if (easeTimer > easeTime)
                {
                    //０ならば、遷移時間がないので、目的地に直接移動する
                    if (easeTime == 0)
                    {
                        transform.position = _state.GetDestPosition();
                        transform.rotation = _state.GetDestRotation();
                    }
                    //カメラのステートをデフォルトに戻す
                    easeTimer = 0.0f;
                    cameraSystemState = CameraState.CameraState_Default;

                    return;
                }

                transform.position = Vector3.SmoothDamp(transform.position, _state.GetDestPosition(), ref _velocity, positionDelay);
                transform.rotation = Quaternion.Slerp(transform.rotation, _state.GetDestRotation(), rotationDelay * Time.deltaTime);

            }
            else if (cameraSystemState == CameraState.CameraState_Event)
            {

                easeTimer += Time.deltaTime;

                // 割合を 0.0 ～ 1.0 の範囲に制限
                float easeClamp = Mathf.Clamp01(easeTimer / easeTime);

                // SmoothStepを使って、動き出しと止まる瞬間をより滑らか（イージング）にする
                float easeBlend = Mathf.SmoothStep(0.0f, 1.0f, easeClamp);



                if (easeTimer >= easeTime)
                {
                    easeTimer = 0.0f;
                    _eventCamera.Finish();

                    if (_eventCamera.IsCameraFreeAtFinish())
                    {
                        cameraSystemState = CameraState.CameraState_Default;//通常のカメラ制御に戻す
                    }
                    else
                    {
                        cameraSystemState = CameraState.CameraState_Stop;//次のイベント処理まで停止
                    }

                }

                if (_eventCamera.IsCameraFreeAtFinish() == false)
                {
                    // 4. 最終的なカメラ位置と回転を適用
                    transform.position = Vector3.Lerp(eventStartPos, eventDestPos, easeBlend);
                    transform.rotation = Quaternion.Slerp(eventStartRot, eventDestRot, easeBlend);
                }
                else
                {
                    //イベントに入ったときの座標に戻す
                    transform.position = Vector3.Lerp(eventStartPos, eventFinalDestPos, easeBlend);
                    transform.rotation = Quaternion.Slerp(eventStartRot, eventFinalDestRot, easeBlend);
                }

            }
            else if (cameraSystemState == CameraState.CameraState_Stop)
            {

            }



        }
    }

    //==============================================================
    //トリガー登録コールバック関数。プレイヤー侵入、退出時にトリガーによって呼び出される。
    //==============================================================
    public void Register(CameraTrigger trigger)
    {
        m_pCameraStateID++;//インクリメントしてIDとして登録
        trigger.SetCameraStateID(m_pCameraStateID);
        _cameraTriggers.TryAdd(m_pCameraStateID, trigger);//連想配列に登録

        ICameraState cameraState = trigger.GetCameraState();
        cameraState.SetCameraObject(this.gameObject);//カメラオブジェクトを設定
        cameraState.SetMainTarget(_mainTarget);//プレイヤーをターゲットに指定

        cameraState.Init();//ステートの初期化
    }

    public void UnRegister(CameraTrigger trigger)
    {
        uint id = trigger.GetCameraStateID();//トリガーが保持するIDを取得
        if (_cameraTriggers.ContainsKey(id))//一応あるか見てから
        {
            _cameraTriggers.Remove(id);//消す
        }


    }

    //==============================================================
    //イベント呼び出し関数。イベントシステムから呼び出される。
    //==============================================================

    public void EventCallBack(Event_Camera eventCamera)
    {
        prevCameraSystemState = cameraSystemState;
        cameraSystemState = CameraState.CameraState_Event;

        //もしカメラステートが初めて切り替わったら
        if (prevCameraSystemState != CameraState.CameraState_Stop
            && prevCameraSystemState != cameraSystemState)
        {
            //イベントに入る前の座標を保存する
            eventFinalDestPos = transform.position;
            eventFinalDestRot = transform.rotation;
        }

        //イベントから目的地の座標と回転を取得する
        _eventCamera = eventCamera;
        eventStartPos = transform.position;
        eventStartRot = transform.rotation;


        eventDestPos = eventCamera.GetDestPosition();
        eventDestRot = eventCamera.GetDestRotation();

        //イベントから遷移にかかる時間を取得する
        easeTime = eventCamera.GetTransitionTime();
        easeTimer = 0.0f;
    }


    //登録または登録解除時に次のステートを決定するために呼ばれる
    private void SolveCameraState()
    {
        uint resultPriority = 0;
        CameraTrigger resultTrigger = null;

        foreach (var it in _cameraTriggers)
        {
            uint priority = it.Value.GetPriority();

            if (resultPriority <= priority)
            {
                resultPriority = priority;
                resultTrigger = it.Value;
            }

        }

        if (resultTrigger != null)
        {

            _prevState = _state;
            _state = resultTrigger.GetCameraState();

            //違うステートならば
            if (_prevState != _state)
            {
                //トリガーから遷移にどれだけ時間をかけるかの設定を取得
                easeTime = resultTrigger.GetTransitionTime();
                easeTimer = 0.0f;

                prevCameraSystemState = cameraSystemState;
                //システムは補間モードに遷移する
                cameraSystemState = CameraState.CameraState_Interpolate;
            }
        }
    }
}
