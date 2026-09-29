using NUnit.Framework;
using System.Threading;
using System.Collections.Generic;
using UnityEngine;

public class FireworkLaunhcer : MonoBehaviour
{
    [SerializeField] List<GameObject> fireworks;
    [SerializeField] float minInterval = 1;
    [SerializeField] float maxInterval=5;
    [SerializeField] AudioSource source;
    [SerializeField] AudioClip explosionSE;
    float interval;
    float timer = 0.0f;

    private void Awake()
    {
        interval = maxInterval;
    }
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        interval = 0.0f;
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime;
        if(timer>interval)
        {
            Instantiate(fireworks[Random.Range(0, fireworks.Count)], transform.position, Quaternion.identity);//ƒ‰ƒ“ƒ_ƒ€‚Å’Š‘I‚µA‘Å‚¿ã‚°‚é
            source.PlayOneShot(explosionSE);
            interval = Random.Range(minInterval, maxInterval);
            timer = 0.0f;
        }
    }
}
