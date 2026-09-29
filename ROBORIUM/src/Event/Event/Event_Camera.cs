using UnityEngine;



/// <summary>
///特定の場所から特定の場所へカメラが移動するイベント 
/// </summary>
public class Event_Camera : IEvent
{
    [SerializeField] private bool isCameraFreeAtFinish = false;
    [SerializeField] private float transitionTime = 0.0f;
    [SerializeField] private Transform pivotTransform;
    [SerializeField] private Transform targetTransform;

    [SerializeField] protected Mesh pivotMesh;
    [SerializeField] protected Mesh targetMesh;

    float currentBlend = 0f; // 0.0 から 1.0 までの割合

    private void OnDrawGizmosSelected()
    {

        Vector3 tPos = targetTransform.position;
        Vector3 pPos = pivotTransform.position;

        Gizmos.color = new Color(1, 0, 1, 1);//ピンク色
        Vector3 cameraToTarget = (pPos - tPos).normalized;
        //どれだけ回転すればいいかを理想の四元数として算出
        Quaternion rot = Quaternion.LookRotation(cameraToTarget, Vector3.up);
        Gizmos.DrawWireMesh(pivotMesh, pPos, rot);
        Gizmos.DrawWireMesh(targetMesh, tPos, Quaternion.identity);
        Gizmos.DrawLine(tPos, pPos);
        
    }

    public override void Init()
    {
        base.Init();

        CameraSystem.Instance.EventCallBack(this);
    }

    public Vector3 GetDestPosition()
    {
        return pivotTransform.position;
    }

    public Quaternion GetDestRotation()
    {
        Vector3 cameraToTarget = (targetTransform.position - pivotTransform.position).normalized;
        return Quaternion.LookRotation(cameraToTarget, Vector3.up);
    }

    public float GetTransitionTime()
    {
        return transitionTime;
    }

    public bool IsCameraFreeAtFinish()
    {
        return isCameraFreeAtFinish;
    }

    public override void Run()
    {
        base.Run();


    }

    public override void Finish()
    {
        base.Finish();
    }

}
