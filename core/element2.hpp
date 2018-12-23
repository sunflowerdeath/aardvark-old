
class Element {
 public:
  // -------------------------------------------------------------
  // Values of these fields can be set in the default constructor.
  // -------------------------------------------------------------

  // Element should set this to true when its size depends only on 
  // input constraints. Such elements can be relayout boundaries.
  bool size_depends_on_parent = true;

  // Repaint boundary element does not share layers with another elements.
  // This allows to repaint this element separately.
  // This prop should be set explicitly.
  bool is_repaint_boundary = false;

  // --------------------------------------------------------------
  // Methods that can be overriden to implement different elements.
  // --------------------------------------------------------------

  // Name of the element for debugging
  virtual std::string get_debug_name() { return "Unknown element"; };
 
  // In this method element calculates its size within given constraints,
  // layout its children and set their size and relative positions.
  virtual Size layout(BoxConstraints constraints) {};

  // Paints element and its children.
  // `is_changed` is `true` when the element itself or some of its parents 
  // is changed. When it is `false`, element is allowed to reuse result of
  // previous painting.
  virtual void paint(bool is_changed) {};

  virtual void remove_child(std::shared_ptr<Element> child) {};
  virtual void append_child(std::shared_ptr<Element> child) {};
  virtual void insert_before_child(std::shared_ptr<Element> child) {};
  virtual void visit_children(std::function<void(Element*)> visitor) {};

  virtual bool hit_test(double left, double top);

  virtual ResponderBehaviour get_responder_behaviour() {
    return ResponderBehaviour::PassToParent;
  };

  virtual std::optional<Responder*> get_responder() { return std::nullopt; };

  // --------------------------------
  // Something that you interact with
  // --------------------------------
  void change();
  void set_size(Size size);
  void set_rel_position(Position pos);
  void set_clip(std::optional<SkPath> clip);

 private:
  Document* document = nullptr;
  Element* parent = nullptr;
  Position abs_position;
  bool is_relayout_boundary = false;
  std::shared_ptr<LayerTree> layer_tree;
  BoxConstraints prev_constraints;
  bool is_changed;
