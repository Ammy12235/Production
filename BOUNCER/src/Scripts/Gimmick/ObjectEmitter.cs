using UnityEngine;

public class ObjectEmitter : MonoBehaviour
{
    [SerializeField] private GameObject obj;
    [SerializeField] private float interval;

    float timer = 0.0f;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime;
        if(timer>interval)
        {
            Instantiate(obj, transform.position, Quaternion.identity);
            timer = 0.0f;
        }
    }
}
