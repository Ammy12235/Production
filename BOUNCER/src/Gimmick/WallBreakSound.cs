using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WallBreakSound : MonoBehaviour
{
    [SerializeField]AudioClip breakSound;
    AudioSource source;
    // Start is called before the first frame update
    void Start()
    {
        source = GetComponent<AudioSource>();
        source.PlayOneShot(breakSound);
        
    }

    // Update is called once per frame
    void Update()
    {
        if (!source.isPlaying) Destroy(this.gameObject);
    }
}
