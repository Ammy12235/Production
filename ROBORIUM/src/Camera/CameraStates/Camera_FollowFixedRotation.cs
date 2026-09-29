using UnityEngine;



public class Camera_FollowFixedRotation : ICameraState
{

    [SerializeField] private Vector3 positionOffset;

    private void OnDrawGizmosSelected()
    {
        Gizmos.color = new Color(0, 1, 1, 1);//水色

        Vector3 pos = positionOffset+transform.position;
        Vector3 cameraToTarget = (pos - transform.position).normalized;
        //どれだけ回転すればいいかを理想の四元数として算出
        Quaternion rot = Quaternion.LookRotation(cameraToTarget, Vector3.up);
        Gizmos.DrawWireMesh(pivotMesh, pos, rot);
        Gizmos.DrawWireMesh(targetMesh, transform.position, Quaternion.identity);
        Gizmos.DrawLine(transform.position, pos);
    }
    public override void Init()
    {
        _destPosition = _targetObject.transform.position + positionOffset;

        Vector3 cameraToTarget = (_targetObject.transform.position - _cameraObject.transform.position).normalized;
        //どれだけ回転すればいいかを理想の四元数として算出
        _destRotation = Quaternion.LookRotation(cameraToTarget, Vector3.up);

    }
    public override void Run()
    {

        _destPosition = _targetObject.transform.position + positionOffset;

        Vector3 cameraToTarget = (_targetObject.transform.position - _cameraObject.transform.position).normalized;
        //どれだけ回転すればいいかを理想の四元数として算出
        _destRotation = Quaternion.LookRotation(cameraToTarget, Vector3.up);

    }
}
