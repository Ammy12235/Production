using System;
using System.Collections.Generic;
using System.Linq;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;

public class StartEventManager : MonoBehaviour
{
    [Header("遷移先シーン（Build Settings に登録しておく）")]
    [SerializeField] private string sceneName;

    [Header("遷移開始までの待機時間（秒）")]
    [SerializeField] private float waitTime = 1.0f;

    [Header("遷移のフェードアウト時間（秒）")]
    [SerializeField] private float fadeOutTime = 1.0f;

    [Header("フェードアウト用の黒い画像")]
    [SerializeField] private Image fadeOutImage;
    
    [Header("Bボタンを押したときのSE")]
    [SerializeField] private AudioClip pressSE;

    [Header("BボタンUIAnimator")]
    [SerializeField] private Animator pressedBAnim;
    [SerializeField] private Animator pressedBAnim2;

    private bool hasStartedTransition = false;
    private float waitTimer = 0.0f;
    private bool isFadingOut = false;
    private bool isSceneLoading = false;
    
    private AudioSource audioSource;

    void Start()
    {
        PadAssigner.Instance.ResetAssignments();
        audioSource = GetComponent<AudioSource>();
        pressedBAnim2.enabled = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (OnPressedNext() || hasStartedTransition)
        {
            if (!hasStartedTransition)
            {
                hasStartedTransition = true;
                pressedBAnim.SetBool("isPressedB", true);
                pressedBAnim2.enabled = true;
            }
            waitTimer += Time.deltaTime;
            if (!isFadingOut)
            {
                isSceneLoading = false;
                if (waitTimer >= waitTime)
                {
                    isFadingOut = true;
                    waitTimer = 0.0f;
                    if (audioSource != null && pressSE != null)
                    {
                        audioSource.PlayOneShot(pressSE);
                    }
                }
            }
            else
            {
                float alpha = Mathf.Clamp01(waitTimer / fadeOutTime);
                fadeOutImage.color = new Color(0, 0, 0, alpha);
                if (waitTimer > fadeOutTime + 0.5f&& isSceneLoading==false)
                {
                   
                    isSceneLoading = true;
                    LoadManager.instance.NextScene(sceneName);//シーンをロードする
                }
            }
        }

    }

    private bool OnPressedNext()
    {
        if (InputUtil.Instance.IsAnyEastPressed()) return true;
        return false;
    }
}
