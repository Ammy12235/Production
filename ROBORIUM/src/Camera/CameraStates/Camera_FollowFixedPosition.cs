using UnityEngine;

public class Camera_FollowFixedPosition : ICameraState
{
    [SerializeField] private Transform _pivotTransform;
    public override void Init()
    {
        _destPosition = _pivotTransform.position;

        Vector3 cameraToTarget = (_targetObject.transform.position - _pivotTransform.transform.position).normalized;
        //どれだけ回転すればいいかを理想の四元数として算出
        _destRotation = Quaternion.LookRotation(cameraToTarget, Vector3.up);
    }
    public override void Run()
    {
        _destPosition=_pivotTransform.position;

        Vector3 cameraToTarget = (_targetObject.transform.position - _pivotTransform.transform.position).normalized;
        //どれだけ回転すればいいかを理想の四元数として算出
        _destRotation = Quaternion.LookRotation(cameraToTarget, Vector3.up);
    }
}
