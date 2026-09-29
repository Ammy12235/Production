using UnityEngine;

public class DestroyAfter: MonoBehaviour {
    public float delay = 10.0f;
    
    void Start() {
        Destroy(gameObject, delay);
    }    
}
