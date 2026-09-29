using UnityEngine;

public class FollowObject : MonoBehaviour
{
    [SerializeField] private GameObject target;
    [SerializeField] private float rotationOffsetY = 0f;
    [SerializeField] private bool isInverse = false;

    void Update()
    {
        Vector3 p;
        if (target == null)
        {
            return;
        }
        else p = target.transform.position;

        Vector3 diff = (p - transform.position).normalized;
        if (isInverse) diff *= -1;
        transform.LookAt(transform.position + diff);
        transform.Rotate(0, rotationOffsetY, 0);
    }
}
