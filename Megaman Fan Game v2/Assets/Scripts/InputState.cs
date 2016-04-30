using UnityEngine;
using System.Collections;
using UnityStandardAssets.CrossPlatformInput;

public class InputState : MonoBehaviour {

    public float VelX = 0f;
    public float absVelY = 0f;
    public bool standing;
    public float standingThreshold = 1;
    public float speed = 5000f;
    public float maxSpeed = 3;

    private Rigidbody2D body2d;


    void Awake()
    {
        body2d = GetComponent<Rigidbody2D>();

    }

    //Controls which way the player is facing
    void Update()
    {
        if (CrossPlatformInputManager.GetAxis("Horizontal") < -0.1f)
        {
            transform.localScale = new Vector3(.75f, .75f, 1);
        }

        if (CrossPlatformInputManager.GetAxis("Horizontal") > 0.1f)
        {
            transform.localScale = new Vector3(.75f, .75f, 1);
        }

    }

    //Controls player movement
    void FixedUpdate()
    {

        VelX = (body2d.velocity.x);
        absVelY = System.Math.Abs(body2d.velocity.y);

        standing = (absVelY <= standingThreshold);

        //WIP Joystick controls - Josh T. 04/01/2016
        Vector3 easeVelocity = body2d.velocity;
        easeVelocity.y = body2d.velocity.y;

        float h = CrossPlatformInputManager.GetAxis("Horizontal");

        if (standing)
        {
            body2d.velocity = easeVelocity;
        }

        body2d.AddForce((Vector2.right * speed) * h);

        //Limit the speed of the player
        if (body2d.velocity.x > maxSpeed)
        {
            body2d.velocity = new Vector2(maxSpeed, body2d.velocity.y);
        }
       
        if (body2d.velocity.x < -maxSpeed)
        {
            body2d.velocity = new Vector2(-maxSpeed, body2d.velocity.y);

        }
    }
}
