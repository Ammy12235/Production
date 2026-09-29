using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;
using UnityEngine.SceneManagement;
using NUnit.Framework;

public class StuffRoll : MonoBehaviour
{
    [SerializeField] private float scrollSpeed;
    [SerializeField] private RectTransform scrollText;
    [SerializeField] private float stopYPos;
    [SerializeField] private float fadeOutStayTime;
    [Header("フェードアウト用の黒い画像")]
    [SerializeField] private Image fadeOutImage;
    [Header("遷移先シーン（Build Settings に登録しておく）")]
    [SerializeField] private string sceneName;
    [Header("遷移開始までの待機時間（秒）")]
    [SerializeField] private float waitTime = 1.0f;

    [Header("遷移のフェードアウト時間（秒）")]
    [SerializeField] private float fadeOutTime = 1.0f;

    [SerializeField] private List<Sprite> pictures;
    [SerializeField] private Image pictureImage;
    [SerializeField, Header("写真の非表示時間")] private float texEmptyTime;
    [SerializeField, Header("写真のフェードインフェードアウト時間")] private float texFadeTime;
    [SerializeField, Header("写真の表示時間")] private float texStayTime;

    float scrollTimer = 0.0f;
    Vector3 initTextPos;

    bool isFadeOut = false;
    bool isSceneLoading = false;
    enum texState
    {
        state_None = -1,
        state_Empty,
        state_FadeIn,
        state_Stay,
        state_FadeOut,
        state_End,

    }

    texState state;

    float texTimer = 0.0f;
    float waitTimer = 0.0f;
    float stopTimer = 0.0f;
    int pictureIndex = 0;
    AudioSource source;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        isFadeOut = false;
        initTextPos = scrollText.transform.position;
        source = GetComponent<AudioSource>();
        if (source != null)
            source.volume = 1;
        pictureImage.sprite = pictures[0];
        state = texState.state_Empty;
    }

    // Update is called once per frame
    void Update()
    {

        texTimer += Time.deltaTime;
        if (state == texState.state_Empty)
        {

            if (texTimer > texEmptyTime)
            {
                texTimer = 0.0f;
                state = texState.state_FadeIn;
            }
        }
        else if (state == texState.state_FadeIn)
        {
            float alpha = Mathf.Clamp01(texTimer / texFadeTime);
            pictureImage.color = new Color(1, 1, 1, alpha);
            if (texTimer > texFadeTime)
            {
                pictureImage.color = new Color(1, 1, 1, 1);
                texTimer = 0.0f;
                state = texState.state_Stay;

            }
        }
        else if (state == texState.state_Stay)
        {
            if (texTimer > texStayTime)
            {
                texTimer = 0.0f;
                state = texState.state_FadeOut;
            }

        }
        else if (state == texState.state_FadeOut)
        {
            float alpha = Mathf.Clamp01(1 - texTimer / texFadeTime);
            pictureImage.color = new Color(1, 1, 1, alpha);
            if (texTimer > texFadeTime)
            {
                pictureImage.color = new Color(1, 1, 1, 0);
                texTimer = 0.0f;
                pictureIndex++;
                if (pictureIndex >= pictures.Count)
                {
                    state = texState.state_End;
                    return;
                }
                pictureImage.sprite = pictures[pictureIndex];
                state = texState.state_Empty;
            }
        }



        if (stopYPos < scrollText.transform.position.y)
        {
            stopTimer += Time.deltaTime;
            if (stopTimer > fadeOutStayTime)
            {
                stopTimer = 0.0f;
                isFadeOut = true;
            }
        }
        else
        {
            scrollTimer += Time.deltaTime;
            scrollText.transform.position = initTextPos + new Vector3(0, scrollTimer * scrollSpeed, 0);

        }

        if (InputUtil.Instance.IsAnyEastPressed())
        {
            isFadeOut = true;

        }

        if (isFadeOut)
        {
            if (source != null)
                source.volume = Mathf.Clamp01(1 - waitTimer);
            waitTimer += Time.deltaTime;
            float alpha = Mathf.Clamp01(waitTimer / fadeOutTime);
            fadeOutImage.color = new Color(0, 0, 0, alpha);
            if (waitTimer > fadeOutTime && isSceneLoading == false)
            {

                isSceneLoading = true;
                LoadManager.instance.NextScene(sceneName);//シーンをロードする
            }
        }
    }
}
