using UnityEngine;

public class CubeRot : MonoBehaviour
{
    [SerializeField] private float speed=1;
    Vector3 angle;
    Vector3 v;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        angle.x = Random.Range(-100, 100);
        angle.y = Random.Range(-100, 100);
        angle.z = Random.Range(-100, 100);

        v.x = Random.Range(-100, 100)*speed;
        v.y = Random.Range(-100, 100)*speed;
        v.z = Random.Range(-100, 100)*speed;
    }

    // Update is called once per frame
    void Update()
    {
        angle += v/100;
        transform.rotation = Quaternion.Euler(angle);
    }
}
