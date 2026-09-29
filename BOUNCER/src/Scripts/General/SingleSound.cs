using UnityEngine;

// staticにしてモジュール化した方が良いかも
public class SingleSound : MonoBehaviour
{
    AudioSource audioSource;
    float length = 0.0f;
    public bool isPlaying = false;
    public void SetupSound(Vector3 pos, AudioClip clip, float volume = 1.0f)
    {
        this.transform.position = pos;
        audioSource = gameObject.AddComponent<AudioSource>();
        audioSource.PlayOneShot(clip, volume);
        length = clip.length;
        isPlaying = true;
    }

    void Update()
    {
        if (isPlaying)
        {
            length -= Time.deltaTime;
            if (length <= 0.0f)
            {
                Destroy(this.gameObject);
            }
        }
    }
}
