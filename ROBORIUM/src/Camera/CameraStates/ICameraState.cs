using UnityEngine;

/// <summary>
/// カメラの挙動について記述するクラスのインターフェースクラス
/// </summary>
public class ICameraState : MonoBehaviour
{
    [SerializeField] protected Mesh pivotMesh;
    [SerializeField] protected Mesh targetMesh;
    //そのカメラステートに移行した時の初期化
    public virtual void Init() { }
    //そのカメラ実装を実行する
    public virtual void Run() { }

    public void SetMainTarget(GameObject targetObject)
    {
        _targetObject = targetObject;
    }

    public void SetCameraObject(GameObject cameraObject)
    {
        _cameraObject = cameraObject;
    }

    public Vector3 GetDestPosition()
    {
        return _destPosition;
    }

    public Quaternion GetDestRotation()
    {
        return _destRotation;
    }

    protected GameObject _cameraObject = null;
    protected GameObject _targetObject = null;
    protected Vector3 _destPosition = Vector3.zero;
    protected Quaternion _destRotation = Quaternion.identity;
}
