using System.Threading;
using UnityEngine;
public class Camera_FollowFree : ICameraState
{
    [SerializeField] uint distance = 0;
    [SerializeField] private float yOffset = 10;

    private Vector3 targetPos;
    private Vector3 prevTargetPos;

    private PlayerController controller;
    public override void Init()
    {
        targetPos = _targetObject.transform.position;
        prevTargetPos = targetPos;

        //メインターゲットがプレイヤーならば、
        if(_targetObject != null&& _targetObject.tag=="Player")
        {
            //プレイヤーコントローラを取得して、クッション機能に使用する時のために備える
            controller= _targetObject.GetComponent<PlayerController>();
        }
    }

    public override void Run()
    {
        targetPos = _targetObject.transform.position;
        Vector3 diff = targetPos - prevTargetPos;
        Vector3 dir = diff.normalized;

  
        //カメラとターゲットから延びる単位ベクトルを定義
        Vector3 targetToCamera = (_cameraObject.transform.position - _targetObject.transform.position).normalized;
        Vector3 cameraToTarget = -targetToCamera;
      

        //どれだけ前進すればいいかを理想の位置ベクトルとして算出
        _destPosition.x = _targetObject.transform.position.x+targetToCamera.x * distance;
        _destPosition.z = _targetObject.transform.position.z+targetToCamera.z * distance;

        _destPosition.y = _targetObject.transform.position.y+yOffset;

        //どれだけ回転すればいいかを理想の四元数として算出
        _destRotation =Quaternion.LookRotation(cameraToTarget, Vector3.up);

        prevTargetPos = targetPos;
    }

}
