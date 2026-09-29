using UnityEngine;

public class CameraTrigger : MonoBehaviour
{


    [SerializeField] private TriggerShape triggerShape = TriggerShape.Box;
    [SerializeField, Range(0, 100)] uint _priority = 0;//優先度。ほかのトリガーと干渉または内包される関係にあればこの優先度を適用する。
    [SerializeField] float _transitionTime = 0;//AからBのステートに移るときどれだけかかるか
    [SerializeField] bool _triggerOnceFlag = false;//プレイヤー侵入時一度だけ発動するか
    private ICameraState _state;//カメラの挙動
    private ICameraSpecificEaseDetail _cameraEaseDetail;//カメラが遷移する際に特殊な動きをさせたい場合の動き

    private uint _id = 0;
    private bool _isRegistered = false;//プレイヤーが干渉し、システムに登録されているか
    private bool _isActive = true;

    private Collider _collider = null;

    private void Awake()
    {
        _state = GetComponent<ICameraState>();//セットされているステートスクリプトを取得
        _cameraEaseDetail = GetComponent<ICameraSpecificEaseDetail>();//セットされている特殊遷移スクリプトを取得（強制ではない）


        _collider = TriggerUtility.AddTriggerCollider(transform, triggerShape);


        if (_state == null)
        {
            Debug.LogAssertion("CameraTrigger:CameraStateが設定されていません。ICameraStateを継承したCameraStateをオブジェクトに含めてください。");
        }
    }
#if UNITY_EDITOR
    void OnDrawGizmos()
    {
        if (_isActive == false)//機能していないなら
        {
            Gizmos.color = new Color(1, 0, 0, 1);//赤色
        }
        else if (_isRegistered == true)//有効なら
        {
            Gizmos.color = new Color(1, 0.5f, 0, 1);//オレンジ色
        }
        else//普通の状態なら
        {
            Gizmos.color = new Color(0, 1, 1, 1);//水色
        }

        TriggerUtility.DrawTriggerGizmo(transform, triggerShape,1);
    }

    private void OnDrawGizmosSelected()
    {
        if (_isActive == false)//機能していないなら
        {
            Gizmos.color = new Color(1, 0, 0, 1);//赤色
        }
        else if (_isRegistered == true)//有効なら
        {
            Gizmos.color = new Color(1, 0.5f, 0, 1);//オレンジ色
        }
        else//普通の状態なら
        {
            Gizmos.color = new Color(0, 1, 1, 1);//水色
        }

        TriggerUtility.DrawTriggerGizmo(transform, triggerShape, 0.99f);
    }
#endif

    public uint GetPriority()
    {
        return _priority;
    }

    public bool GetIsRegistered()
    {
        return _isRegistered;
    }

    public ICameraState GetCameraState()
    {
        return _state;
    }

    public ICameraSpecificEaseDetail GetCameraEaseDetail()
    {
        return _cameraEaseDetail;
    }

    public float GetTransitionTime()
    {
        return _transitionTime;
    }

    public void SetCameraStateID(uint id)
    {
        _id = id;
    }

    public uint GetCameraStateID()
    {
        return _id;
    }

    private void OnTriggerEnter(Collider other)
    {
        if (_isActive == false) return;

        if (_triggerOnceFlag == true)
        {
            _isActive = false;
        }

        int layer = other.gameObject.layer;

        //もしプレイヤーが侵入したら
        if (layer == LayerMask.NameToLayer("Camera"))
        {
            _isRegistered = true;//登録する
            CameraSystem.Instance.Register(this);
        }
    }

    private void OnTriggerExit(Collider other)
    {
        int layer = other.gameObject.layer;

        //もしプレイヤーが侵入したら
        if (layer == LayerMask.NameToLayer("Camera"))
        {
            _isRegistered = false;//登録解除する
            CameraSystem.Instance.UnRegister(this);
        }
    }
}
