using UnityEngine;

public class LiftPlayerTrigger : MonoBehaviour
{
    [SerializeField] bool isNameDetect = false;
    [SerializeField] string targetName = "";
    [SerializeField] private TriggerShape triggerShape = TriggerShape.Box;

    private Player playerComponent;
    EventEmitter _eventEmitter;

    Collider _collider = null;
    private void Awake()
    {
        _eventEmitter = GetComponent<EventEmitter>();

        _collider = TriggerUtility.AddTriggerCollider(transform, triggerShape);
    }

#if UNITY_EDITOR
    void OnDrawGizmos()
    {

        //普通の状態なら
        Gizmos.color = Color.magenta;//ピンク色

        TriggerUtility.DrawTriggerGizmo(transform, triggerShape, 1);
    }

    private void OnDrawGizmosSelected()
    {
        Gizmos.color = Color.magenta;//ピンク色
        TriggerUtility.DrawTriggerGizmo(transform, triggerShape, 0.99f);
    }
#endif

    private void OnTriggerEnter(Collider other)
    {


        int layer = other.gameObject.layer;

        //もしイベントオブジェクトが侵入したら
        if (layer == LayerMask.NameToLayer("Event"))
        {
            playerComponent = PlayerLocator.Instance.CurrentPlayer.GetComponent<Player>();
            if (playerComponent.IsLiftingObject)
            {
                if (_eventEmitter != null)
                {
                    if (isNameDetect)
                    {
                        if (other.gameObject.name == targetName)
                        {
                            _eventEmitter.Fire();
                            if (_eventEmitter.GetIsFireOnce())
                            {

                                Destroy(gameObject);//イベントトリガーは一度発動したら消える
                            }
                        }

                    }
                    else
                    {

                        _eventEmitter.Fire();
                        if (_eventEmitter.GetIsFireOnce())
                        {

                            Destroy(gameObject);//イベントトリガーは一度発動したら消える
                        }
                    }
                }   
            }
        }
    }
}