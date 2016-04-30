using UnityEngine;
using System.Collections;

public class Bullet : MonoBehaviour
{

    private Rigidbody2D rbody;
    private float startx, starty, endx, endy;
    public int Damage = 1;
    public int maxDistance = 150;


    // Use this for initialization
    void Start()
    {
        rbody = GetComponent<Rigidbody2D>();
        startx = rbody.position.x;
        // starty = rbody.position.y;   - not used JT
    }

    public void SetStartX(float x)
    {
        startx = x;
    }

    // Update is called once per frame
    void Update()
    {
        endx = rbody.position.x;
        // endy = rbody.position.y;         - not used JT

        if (Mathf.Abs(endx - startx) > maxDistance) // Changed 30 to maxDistance
        {
            GameObjectUtil.Destroy(this.gameObject);
        }
    }

    void OnCollisionEnter2D(Collision2D coll)
    {
        if (coll.gameObject.tag == "Enemy" || coll.gameObject.tag == "Wall" ||
            coll.gameObject.tag == "Obstacles" || coll.gameObject.tag == "Floor")
            GameObjectUtil.Destroy(this.gameObject);
    }
}