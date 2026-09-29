
using UnityEngine;
using System.Collections.Generic;
using Unity.VisualScripting;


public enum e2DUIPattern
{
    Pattern_None = -1,
    Pattern_Default,
    Pattern_Config,
    Pattern_OnlyUndo,

}
/// <summary>
/// 補完関数の基底クラス。
/// UIなど位置を補完するオブジェクトを記述。
/// SettingManagerによってSettingUI経由で任意に呼び出される。
/// </summary>
public class EaseFunc : MonoBehaviour
{
    [System.Serializable]
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    public struct easeParameter
    {
        public GameObject easeObj;
        public Transform start;
        public Transform dest;
        public iTween.EaseType easeType;
        public float easeTime;
        public bool isLocal;

    }

  

    [SerializeField] protected List<easeParameter> easeObjs;

    [SerializeField] protected float defaultEaseTime = 1.0f;
    [SerializeField] protected FollowingCamera camera;
    [SerializeField] protected float cameraStartDistance;
    [SerializeField] protected float cameraDestDistance;

    [SerializeField] protected float cameraStartAngle;
    [SerializeField] protected float cameraDestAngle;

    [SerializeField] protected float cameraStartRotSpeed;
    [SerializeField] private float cameraDestRotSpeed;

    [SerializeField] protected float cameraEaseSpeed;
    [SerializeField] protected e2DUIPattern currentGuideUIPattern;
    [SerializeField] protected e2DUIPattern nextGuideUIPattern;

    private float easeLongestTime;

    private float prevDistance;
    private float prevAngle;
    private bool isEaseCalled = false;

    /*
    public void SetStartCameraParam(float distance, float angle)
    {
        startDistance = distance;
        startAngle = angle;
    }
    */

    public void SetStartPos(bool isReverse)
    {


        if (easeObjs == null) return;
        if (isReverse == false)
        {
            prevDistance = camera.GetDistance();
            prevAngle = camera.GetPolarAngle();

            for (int i = 0; i < easeObjs.Count; i++)
            {
                easeObjs[i].easeObj.transform.position = easeObjs[i].start.transform.position;
            }
        }
        else
        {

            for (int i = 0; i < easeObjs.Count; i++)
            {
                easeObjs[i].easeObj.transform.position = easeObjs[i].dest.transform.position;
            }
        }
    }

    public e2DUIPattern GetNextGuideUIPattern()
    {
        return nextGuideUIPattern;
    }

    public e2DUIPattern GetCurrentGuideUIPattern()
    {
        return currentGuideUIPattern;
    }
    public void Start()
    {
        //一番遷移にかかる時間がかかるオブジェクトの遷移時間を保存しておく
        if (easeObjs == null)
        {
            easeLongestTime = defaultEaseTime;
            return;
        }
        float longest = 0;
        float time;
        for (int i = 0; i < easeObjs.Count; i++)
        {
            time = easeObjs[i].easeTime;
            if (time > longest)
            {
                longest = time;
            }
        }
        easeLongestTime = longest;
    }

    public float GetLongestTime()
    {
        return easeLongestTime;
    }

    public virtual void Proceed()
    {


        if (easeObjs == null) return;
        if (isEaseCalled == false)
        {

            for (int i = 0; i < easeObjs.Count; i++)
            {
                Vector3 destPos=Vector3.zero;
                if(easeObjs[i].isLocal)
                {
                    destPos = easeObjs[i].dest.transform.localPosition;
                }
                else destPos = easeObjs[i].dest.transform.position;

                iTween.MoveTo(easeObjs[i].easeObj, iTween.Hash(
                     "position", destPos,
                     "time", easeObjs[i].easeTime,
                     "easetype", easeObjs[i].easeType.ToString(),
                     "isLocal", easeObjs[i].isLocal)
                 );
            }
            isEaseCalled = true;
        }

    }

    //カメラなどの正方向制御はこちらで呼び出すように
    public virtual void LateProceed()
    {
        camera.SetDistance(iTween.FloatUpdate(camera.GetDistance(), cameraDestDistance, cameraEaseSpeed));
        camera.SetPolarAngle(iTween.FloatUpdate(camera.GetPolarAngle(), cameraDestAngle, cameraEaseSpeed));
        camera.SetRotSpeed(iTween.FloatUpdate(camera.GetRotSpeed(), cameraDestRotSpeed, cameraEaseSpeed));
    }

    public virtual void Reverse()
    {

        if (easeObjs == null) return;
        if (isEaseCalled == false)
        {

            for (int i = 0; i < easeObjs.Count; i++)
            {
                Vector3 destPos = Vector3.zero;
                if (easeObjs[i].isLocal)
                {
                    destPos = easeObjs[i].start.transform.localPosition;
                }
                else destPos = easeObjs[i].start.transform.position;

                iTween.MoveTo(easeObjs[i].easeObj, iTween.Hash(
                     "position", destPos,
                     "time", easeObjs[i].easeTime,
                     "easetype", easeObjs[i].easeType.ToString(),
                     "isLocal", easeObjs[i].isLocal)
                 );
            }
            isEaseCalled = true;

        }
    }

    //カメラなどの逆方向制御はこちらで呼び出すように
    public virtual void LateReverse()
    {
        camera.SetDistance(iTween.FloatUpdate(camera.GetDistance(), cameraStartDistance, cameraEaseSpeed));
        camera.SetPolarAngle(iTween.FloatUpdate(camera.GetPolarAngle(), cameraStartAngle, cameraEaseSpeed));
        camera.SetRotSpeed(iTween.FloatUpdate(camera.GetRotSpeed(), cameraStartRotSpeed, cameraEaseSpeed));
    }

    public virtual void Entry()
    {
        camera.SetDistance(cameraStartDistance);
        camera.SetPolarAngle(cameraStartAngle);
        camera.SetRotSpeed(cameraStartRotSpeed);

        isEaseCalled = false;
    }

    public virtual void End()
    {
        camera.SetDistance(cameraDestDistance);
        camera.SetPolarAngle(cameraDestAngle);
        camera.SetRotSpeed(cameraDestRotSpeed);

        isEaseCalled = false;
    }
}
