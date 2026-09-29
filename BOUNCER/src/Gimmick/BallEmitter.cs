using System.Collections.Generic;
using UnityEngine;


public class BallEmitter : MonoBehaviour
{
    [SerializeField] private GameObject ball;
    [SerializeField] private float interval;
    [SerializeField] private List<Material> mats;

    float timer = 0.0f;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime;
        if (timer > interval)
        {

            GameObject obj = Instantiate(ball, transform.position, Quaternion.identity);
            Material mat = obj.GetComponentInChildren<MeshRenderer>().materials[0];
            mat.color = new Color((float)Random.Range(0, 10)/10, 1, (float)Random.Range(0, 10) / 10);
            timer = 0.0f;
        }
    }
}
