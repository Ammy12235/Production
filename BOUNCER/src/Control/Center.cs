using UnityEngine;

public class Center : MonoBehaviour
{
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }
    public Transform target;
    // Update is called once per frame
    void Update()
    {
        iTween.MoveUpdate(this.gameObject, iTween.Hash(
               "position", target.position,
               "time", 3.0f,
               "easeType", "easeOutBounce"
               )
           );
        iTween.RotateUpdate(this.gameObject, iTween.Hash(
            "rotation", target.rotation.eulerAngles,
            "time", 3.0f)
        );
    }
}
