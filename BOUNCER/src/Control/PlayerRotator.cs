using UnityEngine;

public class PlayerRotator : MonoBehaviour
{
    readonly private float initRotateSpeed = 2;
    readonly private float destRotateSpeed = 0.04f;
    float rotateSpeed;
    Quaternion initRotation;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        initRotation = transform.rotation;
        rotateSpeed = initRotateSpeed;
    }

    // Update is called once per frame
    void Update()
    {
        rotateSpeed = iTween.FloatUpdate(rotateSpeed, destRotateSpeed, 7);
        transform.Rotate(new Vector3(0, rotateSpeed*Time.deltaTime*Application.targetFrameRate, 0));
    }
    private void OnDestroy()
    {
        transform.rotation = initRotation;
    }
}
