
using UnityEngine;

public class IsExist : MonoBehaviour
{

    [SerializeField] private bool isCompare = false;
    [SerializeField] private GameObject targetObj;
    string target;
    bool isExist = false;

    bool isEnter = false;
    bool isStay = false;
    bool isExit = false;


    // Start is called before the first frame update
    void Start()
    {
        if (targetObj != null)
            target = targetObj.tag;
        isEnter = false;
        isStay = false;
        isExit = false;

        isExist = false;
    }

    public void SetTargetTag(string tag)
    {
        target = tag;
    }

    public void SetTarget(GameObject obj)
    {
        targetObj = obj;
        target = obj.tag;
    }

    public bool IsExistObj()
    {
        return isExist;
    }

    public GameObject getTarget()
    {
        if (targetObj != null)
            return targetObj;
        else return null;
    }

    // Update is called once per frame
    void Update()
    {
        if (isEnter || isStay)
        {
            isExist = true;
        }
        else
        {
            isExist = false;
        }

        isEnter = false;
        isStay = false;
        isExit = false;
    }

    private void OnTriggerEnter(Collider other)
    {

        if (isCompare)
        {
            if (other.CompareTag(target))
            {
                isEnter = true;
            }
        }
        else isEnter = true;

    }

    private void OnTriggerStay(Collider other)
    {

        if (isCompare)
        {
            if (other.CompareTag(target))
            {
                isStay = true;

            }
        }
        else isStay = true;
    }

    private void OnTriggerExit(Collider other)
    {

        if (isCompare)
        {
            if (other.CompareTag(target))
            {
                isExit = true;
            }
        }
        else isExit = true;
    }
}
