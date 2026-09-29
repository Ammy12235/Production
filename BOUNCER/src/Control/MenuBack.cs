using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;

public class MenuBack : MonoBehaviour
{
    [SerializeField] private RawImage menuBack;
    [SerializeField] private RawImage textBack;
    [SerializeField] private float fadeSpeed;

    bool isToOne = false;
    bool isToZero = false;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (isToZero)
        {
            menuBack.color = new Color(1, 1, 1, iTween.FloatUpdate(menuBack.color.a, 0, fadeSpeed));
            textBack.color = new Color(1, 1, 1, iTween.FloatUpdate(textBack.color.a, 0, fadeSpeed));
            if (menuBack.color.a < 0.001)
            {
                menuBack.color = new Color(1, 1, 1, 0);
                textBack.color = new Color(1, 1, 1, 0);
                isToZero = false;
            }
        }
        else if (isToOne)
        {
            menuBack.color = new Color(1, 1, 1, iTween.FloatUpdate(menuBack.color.a, 1, fadeSpeed));
            textBack.color = new Color(1, 1, 1, iTween.FloatUpdate(textBack.color.a, 1, fadeSpeed));
            if(menuBack.color.a>0.999)
            {
                menuBack.color = new Color(1, 1, 1, 1);
                textBack.color = new Color(1, 1, 1, 1);
                isToOne = false;
            }
        }
        
    }

    //alpha’l
    public void ToOne()
    {
        menuBack.color = new Color(1, 1, 1, 0);
        textBack.color = new Color(1, 1, 1, 0);
        isToOne = true;
        isToZero = false;
    }
    public void ToZero()
    {
        menuBack.color = new Color(1, 1, 1, 1);
        textBack.color = new Color(1, 1, 1, 1);
        isToOne = false;
        isToZero = true;
    }
}
