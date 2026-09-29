using NUnit.Framework.Internal;
using Unity.VisualScripting;
using UnityEngine;

public class Camera_Fixed : ICameraState
{
    [SerializeField] private Transform _targetTransform;
    [SerializeField] private Transform _pivotTransform;

    private void OnDrawGizmosSelected()
    {

        Vector3 tPos = _targetTransform.position;
        Vector3 pPos = _pivotTransform.position;

        Gizmos.color = new Color(0, 1, 1, 1);//水色
        Vector3 cameraToTarget = (pPos - tPos).normalized;
        //どれだけ回転すればいいかを理想の四元数として算出
        Quaternion rot = Quaternion.LookRotation(cameraToTarget, Vector3.up);
        Gizmos.DrawWireMesh(pivotMesh, pPos, rot);
        Gizmos.DrawWireMesh(targetMesh, tPos, Quaternion.identity);
        Gizmos.DrawLine(tPos, pPos);
        int max = 10;
        for (int i = 0; i < max; i++)
        {

            Vector3 start = Vector3.Lerp(transform.position, tPos, (float)i / max);
            Vector3 dest = Vector3.Lerp(transform.position, tPos, (float)i / max + ((float)1 / max) * 0.2f);
            Gizmos.DrawLine(start, dest);



            start = Vector3.Lerp(transform.position, pPos, (float)i / max);
            dest = Vector3.Lerp(transform.position, pPos, (float)i / max + ((float)1 / max) * 0.2f);
            Gizmos.DrawLine(start, dest);

        }
    }

    public override void Init()
    {

    }
    public override void Run()
    {
        Vector3 cameraToTarget = (_targetTransform.position - _cameraObject.transform.position).normalized;

        //どれだけ前進すればいいかを理想の位置ベクトルとして算出
        _destPosition = _pivotTransform.position;
        //どれだけ回転すればいいかを理想の四元数として算出
        _destRotation = Quaternion.LookRotation(cameraToTarget, Vector3.up);
    }

}